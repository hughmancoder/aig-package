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

  struct AigNode {
    Type type;
    
    Lit id; // Literal (id) encodes the node number and inversion bit
    Lit fanin0 = 0; // left node
    Lit fanin1 = 0; // right node
  };

  // Parse an ASCII AAG from input stream:
  // Format: aag M I L O A  followed by I input lits, L latches, O outputs, A Ands
  void parse(std::istream &in);

  // Accessors
  const std::vector<Lit> &get_inputs() const { return inputs_; }
  const std::vector<Lit> &get_outputs() const { return outputs_; }
  const std::vector<AigNode> &get_nodes() const { return nodes_; }

  // validate AIG structure
  // bool is_topologically_sorted() const;
  
  void display_truth_table(const std::vector<std::vector<bool>> &tt) const;

  // Simulate all 2^n assignments of inputs and generate truth table
   std::vector<std::vector<bool>> generate_truth_table() const;

private:
  void parse_aag_and_construct(std::istream &in);
  void add_input_node(Lit id);
  void add_output_node(Lit id);
  void add_and_gate(Lit id, Lit l, Lit r);

  std::vector<Lit> inputs_;
  std::vector<Lit> outputs_;
  std::vector<AigNode> nodes_;
  std::unordered_map<Lit, AigNode> id_node_map_;
};

#endif 
