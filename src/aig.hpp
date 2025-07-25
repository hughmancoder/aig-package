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

  int get_node_id(Lit lit) const { return lit >> 1; } // extract node id from literal by bit shift (ignoring phase bit). Same as `lit // 2`
  bool is_inverted(Lit lit) const { return lit & 1u; } // if LSB = 1 then the literal is odd so it is inverted
  int get_canonical_lit(Lit lit) const {
    return lit & ~1u;
  }

  struct AigNode {
    Type type;

    Lit literal;    // Literal (id) encodes the node number and inversion bit
    Lit fanin0 = 0; // left node
    Lit fanin1 = 0; // right node
  };

  /*
  Parse an ASCII AAG from input stream:
  Format: aag M I L O A  followed by I input lits, L latches, O outputs, A Ands
  */
  void parse(std::istream &in);

  // Accessors
  const std::vector<Lit> &get_inputs() const { return inputs_; }
  const std::vector<Lit> &get_outputs() const { return outputs_; }
  const std::vector<AigNode> &get_nodes() const { return nodes_; }

  // Compute depth of AIG
  int compute_depth() const;
  // recursive helper
  std::size_t depth_of(Lit lit,
                    std::unordered_map<Lit,std::size_t> &memo) const;

  // Gets number of fanouts for each node
  std::unordered_map<Lit, std::size_t> compute_fanout_counts() const;

  // print fanous count + and aig depth
  void print_stats() const;

  /*
   Checks if the AIG structure is topologically sorted,
   ensuring every node appears after all of its dependencies.
   */
  bool is_topologically_sorted();

  /*
  Generate a truth table from the AIG node structure by simulating all
  assignments
  */
  std::vector<std::vector<bool>> generate_truth_table() const;

  /*
  Helper function to display the truth table vector in a readable format
  */
  void display_truth_table(const std::vector<std::vector<bool>> &tt,
                           size_t numInputs) const;

  // == helper methods ==

  /*
  Generates a boolean vector truth table for all possible input combinations in ascending order
  */
  std::vector<std::vector<bool>> enumerate_inputs(size_t numInputs) const;

/*
  for a given input vector, evaluates the AIG and returns the output vector
  */
  std::vector<bool> evaluate_aig(const std::vector<bool> &inputBits) const;

  /*
  */

private:
  void parse_aag_and_construct(std::istream &in);
  void add_input_node(Lit id);
  void add_output_node(Lit id);
  void add_and_gate(Lit id, Lit l, Lit r);

  std::vector<Lit> inputs_;
  std::vector<Lit> outputs_;
  std::vector<AigNode>
      nodes_; // stores all nodes in the AIG in order of their appearance
  std::unordered_map<Lit, AigNode> id_node_map_;
};

#endif
