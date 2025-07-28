#pragma once
#include "aig.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// reduce structural redundancies
static inline Lit
rewrite_lit(Lit lit, const std::unordered_map<Lit, Lit> &rep_of, const Aig &a) {
  if (lit <= 1)
    return lit;
  Lit can = a.get_canonical_lit(lit);
  auto it = rep_of.find(can);
  if (it == rep_of.end())
    return lit;
  Lit new_can = it->second;
  return a.is_inverted(lit) ? (new_can | 1u) : new_can;
}

Aig sweep_aig(const Aig &in) {
  Aig sweeped_aig;

  // ===== 1) Reachability mark from outputs (keep inputs always) =====

  auto nodes = in.get_nodes();
  std::unordered_map<Lit, Aig::AigNode> and_map;
  and_map.reserve(nodes.size());
  for (const auto &n : nodes) {
    if (n.type == Aig::Type::AND) {
      and_map.emplace(in.get_canonical_lit(n.literal), n);
    }
  }

  std::unordered_set<Lit> reachable_even; // canonical LHS of reachable ANDs
  reachable_even.reserve(and_map.size());

  std::function<void(Lit)> mark = [&](Lit lit) {
    if (lit <= 1)
      return; // constants
    Lit can = in.get_canonical_lit(lit);
    auto it = and_map.find(can);
    if (it == and_map.end())
      return; // inputs or undefined => leaf
    if (!reachable_even.insert(can).second)
      return; // already visited
    mark(it->second.fanin0);
    mark(it->second.fanin1);
  };

  for (Lit outL : in.get_outputs())
    mark(outL);

  // ===== 2) Keep only reachable ANDs; preserve INPUT/OUTPUT nodes =====
  std::vector<Aig::AigNode> kept_and_nodes;
  kept_and_nodes.reserve(reachable_even.size());
  for (const auto &n : in.get_nodes()) {
    if (n.type == Aig::Type::AND) {
      if (reachable_even.count(in.get_canonical_lit(n.literal))) {
        kept_and_nodes.push_back(n);
      }
    }
  }
  

  // ===== 3) Structural hashing for duplicates (commutative AND) =====
  struct PairHash {
    std::size_t operator()(const std::pair<Lit, Lit> &p) const noexcept {
      return std::hash<uint64_t>{}((uint64_t(p.first) << 32) ^ p.second);
    }
  };

  // key: normalized (a,b) -> representative canonical LHS
  std::unordered_map<std::pair<Lit, Lit>, Lit, PairHash> key2rep;
  key2rep.reserve(kept_and_nodes.size());

  // old canonical -> representative canonical
  std::unordered_map<Lit, Lit> rep_of;
  rep_of.reserve(kept_and_nodes.size());

  std::vector<Aig::AigNode> survivors;
  survivors.reserve(kept_and_nodes.size());

  // Two-phase: first pass assigns representatives while rewriting children
  for (auto n : kept_and_nodes) {
    // rewrite children through any known reps (transitive-friendly)
    Lit a = rewrite_lit(n.fanin0, rep_of, in);
    Lit b = rewrite_lit(n.fanin1, rep_of, in);
    if (a > b)
      std::swap(a, b);
    auto key = std::make_pair(a, b);

    Lit lhs_can = in.get_canonical_lit(n.literal);
    auto it = key2rep.find(key);
    if (it == key2rep.end()) {
      // First time we see this structure: make it representative
      n.fanin0 = a;
      n.fanin1 = b;
      key2rep.emplace(key, lhs_can);
      rep_of[lhs_can] = lhs_can; // self
      survivors.push_back(n);
    } else {
      // Duplicate structure: map lhs to existing representative
      rep_of[lhs_can] = it->second;
    
    }
  }

  // ===== 4) Rewrite all consumers (survivors' fanins and outputs) through
  // final reps ===== Path compression for rep_of (to ensure direct reps)
  std::function<Lit(Lit)> find_rep = [&](Lit can) -> Lit {
    auto it = rep_of.find(can);
    if (it == rep_of.end() || it->second == can)
      return can;
    return rep_of[can] = find_rep(it->second);
  };
  for (auto &kv : rep_of)
    kv.second = find_rep(kv.first);

  for (auto &n : survivors) {
    n.fanin0 = rewrite_lit(n.fanin0, rep_of, in);
    n.fanin1 = rewrite_lit(n.fanin1, rep_of, in);
  }
  std::vector<Lit> rewritten_outputs;
  rewritten_outputs.reserve(in.get_outputs().size());
  for (auto outL : in.get_outputs()) {
    rewritten_outputs.push_back(rewrite_lit(outL, rep_of, in));
  }

  // ===== 5) Serialize reduced network to .aag and parse back into an Aig =====
  // We keep the original inputs (order & identities) unchanged.
  std::stringstream ss;

  // Compute M (max variable index) across all used literals
  unsigned M = 0;
  auto bump = [&](Lit lit) {
    if (lit > 1)
      M = std::max(M, static_cast<unsigned>(in.get_variable_index(lit)));
  };
  for (auto lit : in.get_inputs())
    bump(in.get_canonical_lit(lit));
  for (auto lit : rewritten_outputs)
    bump(lit);
  for (const auto &n : survivors) {
    bump(n.literal);
    bump(n.fanin0);
    bump(n.fanin1);
  }

  const std::size_t I = in.get_inputs().size();
  const std::size_t L = 0;
  const std::size_t O = rewritten_outputs.size();
  const std::size_t A = survivors.size();

  ss << "aag " << M << " " << I << " " << L << " " << O << " " << A << "\n";
  for (auto inLit : in.get_inputs())
    ss << in.get_canonical_lit(inLit) << "\n";
  // no latches
  for (auto outLit : rewritten_outputs)
    ss << outLit << "\n";

  // Sort ANDs by variable index for stable output
  std::vector<Aig::AigNode> sorted = survivors;
  std::sort(sorted.begin(), sorted.end(),
            [&](const Aig::AigNode &a, const Aig::AigNode &b) {
              return in.get_variable_index(a.literal) <
                     in.get_variable_index(b.literal);
            });

  for (const auto &n : sorted) {
    ss << n.literal << " " << n.fanin0 << " " << n.fanin1 << "\n";
  }

  Aig out;
  {
    std::string s = ss.str();
    std::istringstream inbuf(s);
    out.parse(inbuf);
  }

  sweeped_aig = std::move(out);
  return sweeped_aig;
}
