#ifndef AIG_HPP
#define AIG_HPP

#include <cstdint>
#include <istream>
#include <unordered_map>
#include <vector>

using Lit = uint32_t;

class Aig {
public:
  enum class Type { INPUT, AND, OUTPUT };

  int get_variable_index(Lit lit) const { return static_cast<int>(lit >> 1); }
  
  bool is_inverted(Lit lit) const { return (lit & 1u) != 0; }

  // Phase agnostic literal (even). Use to group nodes
  Lit get_canonical_lit(Lit lit) const { return lit & ~1u; }

  struct AigNode {
    Type type;

    Lit literal;    // even for input / and
    Lit fanin0 = 0; // left child literal
    Lit fanin1 = 0; // right child literal
  };

  
  // Parse ASCII .aag from input stream.
  // NOTE: ASCII does not enforce AND order; we handle arbitrary orde
  void parse(std::istream &in);

  // Write the current network to ASCII AIGER (.aag)
  void write_aag(std::ostream& out) const;

  // Accessors
  const std::vector<Lit> &get_inputs() const { return inputs_; }
  const std::vector<Lit> &get_outputs() const { return outputs_; }
  const std::vector<AigNode> &get_nodes() const { return nodes_; }

  int compute_depth() const;

  // Gets number of fan-outs for each node (keyed by canonical literal)
  std::unordered_map<Lit, std::size_t> compute_fanout_counts() const;

  std::size_t get_max_fanout(const std::unordered_map<Lit, std::size_t>& fanout) const;

  void print_stats() const;

  // Generates a boolean vector truth table for all possible input combinations in ascending order
  std::vector<std::vector<bool>> enumerate_inputs(size_t numInputs) const;

  // Generate a truth table from the AIG node structure by simulating all assignments
  std::vector<std::vector<bool>> generate_truth_table() const;

  void display_truth_table(const std::vector<std::vector<bool>> &tt, size_t numInputs) const;

  //for a given input vector, evaluates the AIG and returns the output vector
  std::vector<bool> evaluate_aig(const std::vector<bool> &inputBits) const;

private:
  void parse_aag_and_construct(std::istream &in);
  void add_input_node(Lit id);
  void add_output_node(Lit id);
  void add_and_gate(Lit id, Lit l, Lit r);

  // recursive helper
  std::size_t depth_of(Lit lit,
                       std::unordered_map<Lit, std::size_t> &memo) const;

  std::vector<Lit> inputs_;
  std::vector<Lit> outputs_;

  // in file order (inputs, outputs, ANDs)
  std::vector<AigNode> nodes_; 
  
  // Map from canonical literal to aig node (INPUT/AND)
  std::unordered_map<Lit, AigNode> id_node_map_;
};

#endif


