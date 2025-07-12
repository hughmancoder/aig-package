#include "aig.hpp"
#include <iostream>
#include <string>

void Aig::parse(std::istream &in) { parse_aag_and_construct(in); }

void Aig::parse_aag_and_construct(std::istream &in) {
  std::string header;
  in >> header;
  if (header != "aag") {
    throw std::runtime_error("Expected 'aag' header, got '" + header + "'");
  }

  int M, I, L, O, A;
  in >> M >> I >> L >> O >> A;

  inputs_.reserve(I); // preallocate space of size I in vector
  for (int i = 0; i < I; ++i) {
    Lit lit;
    in >> lit;
    add_input_node(lit);
  }

  // skip latches
  for (int i = 0; i < L; ++i) {
    Lit out_lit, in_lit;
    in >> out_lit >> in_lit;
  }

  outputs_.reserve(O); // preallocate space of size O in vector
  for (int i = 0; i < O; ++i) {
    Lit lit;
    in >> lit;
    add_output_node(lit);
  }

  // AND gates
  nodes_.reserve(nodes_.size() + A);
  for (int i = 0; i < A; ++i) {
    Lit lhs, rhs0, rhs1;
    in >> lhs >> rhs0 >> rhs1;
    add_and_gate(lhs, rhs0, rhs1);
  }
}

void Aig::add_input_node(Lit id) {
  AigNode node;
  node.type = Type::INPUT;
  node.id = id;
  inputs_.push_back(id);
  nodes_.push_back(node);
  id_node_map_[id] = node;
}

void Aig::add_output_node(Lit id) {
  AigNode node;
  node.type = Type::OUTPUT;
  node.id = id;
  node.fanin0 = id; // fanin0 holds the driving literal
  outputs_.push_back(id);
  nodes_.push_back(node);
  id_node_map_[id] = node;
}

void Aig::add_and_gate(Lit id, Lit l, Lit r) {
  AigNode node;
  node.type = Type::AND;
  node.id = id;
  node.fanin0 = l;
  node.fanin1 = r;
  nodes_.push_back(node);
  id_node_map_[id] = node;
}

// TODO: complete
// bool Aig::is_topologically_sorted() const {
//   for (size_t i = 0; i < nodes_.size(); ++i) {
//     std::cout << "Node id = " << nodes_[i].id << std::endl;
//   }
//   return false;
// }

// TODO: test and check
std::vector<std::vector<bool>> Aig::generate_truth_table() const {
  const std::size_t nInputs  = inputs_.size();
  const std::size_t nOutputs = outputs_.size();
  const std::size_t rows     = 1u << nInputs;

  std::vector<std::vector<bool>> table;
  table.reserve(rows);

  
  auto eval_lit = [](Lit lit,
                     const std::unordered_map<Lit, bool>& val) -> bool {
    if (lit == 0) return false;        
    if (lit == 1) return true;         

    bool phase   = lit & 1u;
    Lit  var     = lit & ~1u;          
    auto it      = val.find(var);
    if (it == val.end())
        throw std::logic_error("Literal refers to unevaluated variable");
    return phase ? !it->second : it->second;
  };

  // simulate every assignment
  for (std::size_t mask = 0; mask < rows; ++mask) {
    std::unordered_map<Lit, bool> value;          

    // assign primary inputs
    for (std::size_t i = 0; i < nInputs; ++i) {
      const bool bit = (mask >> i) & 1u;
      value[inputs_[i] & ~1u] = bit;              
    }

    
    for (const auto &node : nodes_) {
      if (node.type != Type::AND) continue;
      bool a = eval_lit(node.fanin0, value);
      bool b = eval_lit(node.fanin1, value);
      value[node.id & ~1u] = a & b;
    }

    
    std::vector<bool> row;
    row.reserve(nInputs + nOutputs);

    for (std::size_t i = 0; i < nInputs; ++i)
      row.push_back((mask >> i) & 1u);

    for (Lit outLit : outputs_)
      row.push_back(eval_lit(outLit, value));

    table.push_back(std::move(row));
  }

  return table;
}

void Aig::display_truth_table(const std::vector<std::vector<bool>>& tt) const {
  for (const auto& row : tt) {
    for (bool val : row) {
      std::cout << val << " ";
    }
    std::cout << "\n";
  }
}