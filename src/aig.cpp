

#include "aig.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <algorithm>

void Aig::parse(std::istream &in) { parse_aag_and_construct(in); }

void Aig::parse_aag_and_construct(std::istream &in) {
  std::string header;
  in >> header;
  if (header != "aag") {
    throw std::runtime_error("Expected 'aag' header, got '" + header + "'");
  }

  int M, I, L, O, A;
  in >> M >> I >> L >> O >> A;

  // Preallocate space in memory for vectors 
  inputs_.reserve(I);
  outputs_.reserve(O);
  nodes_.reserve(static_cast<size_t>(I + O + A));
  id_node_map_.clear();
  id_node_map_.reserve(static_cast<size_t>(I + A));

  // inputs (even literals)
  for (int i = 0; i < I; i++) {
    Lit id;
    in >> id;
    add_input_node(id);
  }

  // skip latches
  for (int i = 0; i < L; i++) {
    Lit out_lit, in_lit;
    in >> out_lit >> in_lit;
  }

  // outputs
  for (int i = 0; i < O; i++) {
    Lit driver;
    in >> driver;
    add_output_node(driver);
  }

  // AND gates
  nodes_.reserve(nodes_.size() + A);
  for (int i = 0; i < A; i++) {
    Lit id, l, r;
    in >> id >> l >> r;
    add_and_gate(id, l, r);
  }
}

void Aig::write_aag(std::ostream& out) const {
  const std::size_t I = inputs_.size();
  const std::size_t L = 0;
  const std::size_t O = outputs_.size();
  std::size_t A = 0;
  for (const auto& n : nodes_) if (n.type == Type::AND) A++;

  auto max_idx = 0u;
  auto increment = [&](Lit lit) {
    if (lit > 1) max_idx = std::max(max_idx, static_cast<unsigned>(get_canonical_lit(lit)));
  };

  for (auto in : inputs_)  increment(in);
  for (auto outL : outputs_) increment(outL);
  for (const auto& n : nodes_) {
    if (n.type == Type::INPUT || n.type == Type::AND) increment(n.literal);
    if (n.type == Type::AND) { increment(n.fanin0); increment(n.fanin1); }
  }

  // Header
  out << "aag " << max_idx << " " << I << " " << L << " " << O << " " << A << "\n";

  // Display Inputs Literals (even)
  for (auto in : inputs_) out << get_canonical_lit(in) << "\n";

  // Latches (none)
  
  // Display outputs literals 
  for (auto outL : outputs_) out << outL << "\n";

  // Extract all the and literals which go below input and output for .aag format
  std::vector<AigNode> ands;
  ands.reserve(A);
  for (const auto& n : nodes_) {
    if (n.type == Type::AND) {
      ands.push_back(n);
    }
  }

  // sort the literals in order and display
  std::sort(ands.begin(), ands.end(),
            [&](const AigNode& a, const AigNode& b) { return get_canonical_lit(a.literal) < get_canonical_lit(b.literal); });

  // format: literal fanin0 fanin1
  for (const auto& n : ands) {
    out << n.literal << " " << n.fanin0 << " " << n.fanin1 << "\n";
  }
}

void Aig::add_input_node(Lit id) {
  AigNode node;
  node.type = Type::INPUT;
  node.literal = get_canonical_lit(id); // inputs are defined as even literals
  inputs_.push_back(id);
  nodes_.push_back(node);
  id_node_map_[get_canonical_lit(id)] = node;
}

void Aig::add_and_gate(Lit id, Lit l, Lit r) {
  AigNode node;
  node.type = Type::AND;
  node.literal = get_canonical_lit(id); 
  node.fanin0 = l;
  node.fanin1 = r;
  nodes_.push_back(node);
  id_node_map_[get_canonical_lit(id)] = node;
}

void Aig::add_output_node(Lit driver_lit) {
  AigNode node;
  node.type = Type::OUTPUT;
  node.literal = 0; // not a defining literal
  node.fanin0 = driver_lit; // fanin0 holds the driving literal
  outputs_.push_back(driver_lit);
  nodes_.push_back(node);
}

std::size_t Aig::depth_of(Lit lit,
                          std::unordered_map<Lit, std::size_t> &memo) const {

  // constant literals are zero‑depth (true/false)
  if (lit <= 1)
    return 0;

  Lit node_id = get_canonical_lit(lit); 
  
  // check if node_id depth is already computed and return it if so
  auto it = memo.find(node_id);
  if (it != memo.end())
    return it->second;

  // If not an AND depth 0 (we don't count inputs in depth)
  auto nit = id_node_map_.find(node_id);
  if (nit == id_node_map_.end() || nit->second.type == Type::INPUT) {
    return memo[node_id] = 0;
  }

  // otherwise compute the depth of the node
  const auto &node = id_node_map_.at(node_id);

  // recursively compute the depth of left and right children nodes
  int left_depth = depth_of(node.fanin0, memo);
  int right_depth = depth_of(node.fanin1, memo);

  // the depth of the node depends on the max depth of its children plus one to
  // include the node itself. Store in memo so we don't have to recompute if we
  // come across it again
  return memo[node_id] = 1 + std::max(left_depth, right_depth);
}

int Aig::compute_depth() const {
  std::unordered_map<Lit, std::size_t> memo;
  memo.reserve(id_node_map_.size());

  std::size_t maxDepth = 0;

  // for every output literal, compute its depth and memoize it so we don't have
  // to recompute parent depth for other child nodes
  for (Lit out : outputs_) {
    maxDepth = std::max(maxDepth, depth_of(out, memo));
  }

  return static_cast<int>(maxDepth);
}

std::unordered_map<Lit, std::size_t> Aig::compute_fanout_counts() const {
  std::unordered_map<Lit, std::size_t> node_to_fanout_count_map;
  node_to_fanout_count_map.reserve(nodes_.size());

  // lambda function 
  auto increment_parent = [&](Lit lit) {
    // disregard constant literals
    if (lit == 0 || lit == 1) return; 
    node_to_fanout_count_map[get_canonical_lit(lit)]++;
  };

  // loop through all the inner nodes and increment 
  for (const auto &n : nodes_) {
    // stored in order from top to bottom
    if (n.type == Type::AND) { 
      increment_parent(n.fanin0); 
      increment_parent(n.fanin1);
    }
  }
  for (Lit out : outputs_) {
    increment_parent(out);
  }

  return node_to_fanout_count_map;
}

std::size_t Aig::get_max_fanout(const std::unordered_map<Lit, std::size_t>& m) const {
  std::size_t maxFanOut = 0;
  for (const auto &it : m) {
    maxFanOut = std::max(maxFanOut, it.second);
  }
  return maxFanOut;
}

void Aig::print_stats() const { 
   std::size_t andCnt = 0;

   for (const auto &n : nodes_) {
     if (n.type == Type::AND) {
       andCnt++;
     }
   }

  auto node_to_fanout_count_map = compute_fanout_counts();
  std::size_t maxFO = get_max_fanout(node_to_fanout_count_map);

  std::cout << "Inputs       : " << inputs_.size()  << '\n'
            << "Outputs      : " << outputs_.size() << '\n'
            << "AND gates    : " << andCnt          << '\n'
            << "Depth        : " << compute_depth() << '\n'
            << "Max fan‑out  : " << maxFO           << '\n';
  
  return; }

std::vector<std::vector<bool>> Aig::enumerate_inputs(size_t numInputs) const {

  // numberRows = 2^numInputs (each input can be 0 or 1). This is the same as
  // shifting 1 left by n bits as each left shift corresponds to multiplying by
  // 2
  const size_t numRows = 1u << numInputs;

  std::vector<std::vector<bool>> truthTable;
  truthTable.reserve(numRows);

  for (size_t i = 0; i < numRows; ++i) {
    std::vector<bool> row;
    row.reserve(numInputs);

    for (int bit = numInputs - 1; bit >= 0; bit--) {
      row.push_back((i >> bit) &
                    1u); // extract bit from MSB to LSB and push it to the row.
                         // Shifting it extracts the bit at position 'bit'
    }

    truthTable.push_back(std::move(row));
  }
  return truthTable;
}

std::vector<std::vector<bool>> Aig::generate_truth_table() const {
  auto input_boolean = enumerate_inputs(inputs_.size());

  std::vector<std::vector<bool>> table;
  table.reserve(input_boolean.size());

  for (const auto &inBits : input_boolean) {
    std::vector<bool> row = inBits;
    auto outs = evaluate_aig(inBits); // add outputs

    // append generated outputs to the row sutch that truth table row is format
    // inputs, outputs for each row
    row.insert(row.end(), outs.begin(), outs.end());
    // move ensures row should be moved instead of copied by reference
    table.push_back(std::move(row));
  }
  return table;
}

void Aig::display_truth_table(const std::vector<std::vector<bool>> &tt,
                              size_t numInputs) const {
  // If numInputs is not provided (0), use inputs_.size()
  if (numInputs == 0) {
    numInputs = inputs_.size();
  }

  for (const auto &row : tt) {
    for (size_t i = 0; i < row.size(); ++i) {
      std::cout << row[i];
      // Draw seperator | after after last input and index numInputs - 1
      if (i == numInputs - 1) {
        std::cout << " | ";
      } else if (i + 1 < row.size()) {
        std::cout << " ";
      }
    }
    std::cout << "\n";
  }
}

std::vector<bool> Aig::evaluate_aig(const std::vector<bool> &inputBits) const {

  // keep track of what each node evaluates to for given input bits
  std::unordered_map<int, bool> node_id_value;
  node_id_value.reserve(nodes_.size());

  // Initialize inputs of AIG with inputBits
  for (size_t i = 0; i < inputBits.size(); ++i) {
    node_id_value[get_variable_index(inputs_[i])] = inputBits[i];
  }

  // evaluate the AND gate by taking the values of its inputs (bottom up evaluation)
  auto evaluate_aig_node = [&](Lit lit) -> bool {
    if (lit == 0)
      return false; // 0 literal is always false
    if (lit == 1)
      return true; // 1 literal is always true
    bool node_value = node_id_value[get_variable_index(lit)]; // get
    return is_inverted(lit) ? !node_value : node_value;
  };

  // Sweep through all the AND nodes in AIG
  for (const auto &n : nodes_) {
    if (n.type != Type::AND)
      continue;

    // compute child nodes and update parent
    bool lhs = evaluate_aig_node(n.fanin0);
    bool rhs = evaluate_aig_node(n.fanin1);
    node_id_value[get_variable_index(n.literal)] =
        lhs && rhs; // Evaluate AIG node value by taking and of fanin0 and
                    // fanin1 and assigning it to the node id
  }

  // Evaluate outputs
  std::vector<bool> outputs;
  outputs.reserve(outputs_.size());

  // run through all outputs and evaluate them
  for (Lit output : outputs_) {
    outputs.push_back(evaluate_aig_node(output));
  }
  return outputs;
}

/*
// aag format does not require this
bool Aig::is_topologically_sorted() {
std::unordered_map<Lit, size_t> literal_to_position;
// 1) keep track of the position of each node id in a map
for (size_t i = 0; i < nodes_.size(); i++) {
  const auto variable_index = get_variable_index(nodes_[i].literal);
  literal_to_position[variable_index] = i;
}

// 2) verify order of AND gates to that every fanIn before it has lower id
for (size_t i = 0; i < nodes_.size(); i++) {
  // take direct reference to avoid taking copy of nodes
  const auto &n = nodes_[i];
  if (n.type != Type::AND)
    continue; // skip output and input nodes

  // explore children of each And node
  for (Lit fanin : {n.fanin0, n.fanin1}) {
    // skip fist two literals as there are no dependencies
    if (fanin == 0 || fanin == 1)
      continue;
    int node_index = get_variable_index(fanin);

    auto position_iterator = literal_to_position.find(node_index);
    if (position_iterator->second >= i) {
      return false; // child node has greater position than parent node so not
                    // topologically sorted
    }
  }
}
return true;
}
*/