#include "aig.hpp"
#include <cstddef>
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

  outputs_.reserve(O); // preallocate space of size O in vector
  for (int i = 0; i < O; i++) {
    Lit id;
    in >> id;
    add_output_node(id);
  }

  // AND gates
  nodes_.reserve(nodes_.size() + A);
  for (int i = 0; i < A; i++) {
    Lit id, rhs0, rhs1;
    in >> id >> rhs0 >> rhs1;
    add_and_gate(id, rhs0, rhs1);
  }
}

void Aig::add_input_node(Lit id) {
  AigNode node;
  node.type = Type::INPUT;
  node.literal = id;
  inputs_.push_back(id);
  nodes_.push_back(node);
  id_node_map_[id] = node;
}

void Aig::add_and_gate(Lit id, Lit l, Lit r) {
  AigNode node;
  node.type = Type::AND;
  node.literal = id;
  node.fanin0 = l;
  node.fanin1 = r;
  nodes_.push_back(node);
  id_node_map_[id] = node;
}

void Aig::add_output_node(Lit id) {
  AigNode node;
  node.type = Type::OUTPUT;
  node.literal = id;
  node.fanin0 = id; // fanin0 holds the driving literal
  outputs_.push_back(id);
  nodes_.push_back(node);
  id_node_map_[id] = node;
}

bool Aig::is_topologically_sorted() {
  std::unordered_map<Lit, size_t> literal_to_position;
  // 1) keep track of the position of each node id in a map
  for (size_t i = 0; i < nodes_.size(); i++) {
    const auto node_id = get_node_id(nodes_[i].literal);
    literal_to_position[node_id] = i; 
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
      int node_id = get_node_id(fanin);

      auto position_iterator = literal_to_position.find(node_id);
      if (position_iterator->second >= i) {
        return false; // child node has greater position than parent node so not
                      // topologically sorted
      }
    }
  }
  return true;
}

std::vector<std::vector<bool>>
Aig::enumerate_inputs(size_t numInputs) const
{

    // numberRows = 2^numInputs (each input can be 0 or 1). This is the same as shifting 1 left by n bits as each left shift corresponds to multiplying by 2
    const size_t numRows = 1u << numInputs;

    std::vector<std::vector<bool>> truthTable;
    truthTable.reserve(numRows);

    for (size_t i = 0; i < numRows; ++i) {
        std::vector<bool> row;
        row.reserve(numInputs);

        for (int bit = numInputs - 1; bit >= 0; bit--) {
            row.push_back((i >> bit) & 1u); // extract bit from MSB to LSB and push it to the row. Shifting it extracts the bit at position 'bit'
        }

        truthTable.push_back(std::move(row));
    }
    return truthTable;
}

std::vector<bool>
Aig::evaluate_aig(const std::vector<bool>& inputBits) const
{

  // keep track of what each node evaluates to for given input bits
  std::unordered_map<int,bool> node_id_value;
    node_id_value.reserve(nodes_.size());
    
    // Initialize inputs of AIG with inputBits
    for (size_t i = 0; i < inputBits.size(); ++i) {
      node_id_value[get_node_id(inputs_[i])] = inputBits[i];
    }

      // evaluate the AND gate by taking the values of its inputs (bottom up evaluation)
      auto evaluate_aig_node = [&](Lit lit) -> bool {
        if (lit == 0) return false; // 0 literal is always false
        if (lit == 1) return true;  // 1 literal is always true
        bool node_value = node_id_value[get_node_id(lit)]; // get
        return is_inverted(lit) ? !node_value : node_value; 
      };


    // Sweep through all the AND nodes in AIG
     for (const auto& n : nodes_) {
        if (n.type != Type::AND) continue;
    
        // compute child nodes and update parent
        bool lhs = evaluate_aig_node(n.fanin0);
        bool rhs = evaluate_aig_node(n.fanin1);
        node_id_value[get_node_id(n.literal)] = lhs && rhs; // Evaluate AIG node value by taking and of fanin0 and fanin1 and assigning it to the node id
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

std::vector<std::vector<bool>>
Aig::generate_truth_table() const
{
    auto input_boolean = enumerate_inputs(inputs_.size());

    std::vector<std::vector<bool>> table;
    table.reserve(input_boolean.size());

    for (const auto& inBits : input_boolean) {
        std::vector<bool> row = inBits; 
        auto outs  = evaluate_aig(inBits);       // add outputs

        // append generated outputs to the row sutch that truth table row is format inputs, outputs for each row
        row.insert(row.end(), outs.begin(), outs.end());
        // move ensures row should be moved instead of copied by reference
        table.push_back(std::move(row)); 
    }
    return table;
}



void Aig::display_truth_table(const std::vector<std::vector<bool>> &tt, size_t numInputs) const {
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