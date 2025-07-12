// Update the path below to the correct location of aig.hpp if needed
#include "../src/aig.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

int main() { // Minimal AAG 2 inputs, 1 output, and 1 AND gate
  {
    std::istringstream in("aag 3 2 0 1 1\n"
                          "2\n"
                          "4\n"
                          "6\n"
                          "6 2 4\n");
    Aig aig;
    aig.parse(in);
    assert(aig.get_inputs().size() == 2);
    assert(aig.get_outputs().size() == 1);
    // nodes_ holds inputs + outputs + AND gates => 2 + 1 + 1 = 4
    assert(aig.get_nodes().size() == 4);
  }

  // Read and parse and.aag file
  {
    std::ifstream f("examples/and.aag");
    Aig aig;
    aig.parse(f);
    assert(aig.get_inputs().size() == 2);
    assert(aig.get_outputs().size() == 1);

    // assert(aig.is_topologically_sorted());

    size_t and_count = 0;
    for (auto const &n : aig.get_nodes())
      if (n.type == Aig::Type::AND)
        ++and_count;
    assert(and_count == 1);

    auto tt = aig.generate_truth_table();
    aig.display_truth_table(tt);
    // NOTE For debugging purposes
    std::cout << "Truth Table for AND gate:\n";
    

    // std::vector<std::vector<bool>> expected = {// Format A B Y
    //                                            {0, 0, 0},
    //                                            {0, 1, 0},
    //                                            {1, 0, 0},
    //                                            {1, 1, 1}};    
    // assert(tt == expected); // TODO
  }

  // full_adder.aag
  {
    std::ifstream in("examples/full_adder.aag");
    assert(in && "unable to open examples/full_adder.aag");
    Aig aig;
    aig.parse(in);
    assert(aig.get_inputs().size() == 3);
    assert(aig.get_outputs().size() == 2);

    // assert(fa.is_topologically_sorted());
    
    size_t and_count = 0;
    for (auto const &n : aig.get_nodes())
      if (n.type == Aig::Type::AND)
        ++and_count;
    assert(and_count == 11);

    auto tt = aig.generate_truth_table();
    std::cout << "Truth Table for full_adder aig:\n";
    std::cout << "Inputs: A B Cin | Outputs: Cout Sum \n";
    aig.display_truth_table(tt);

    // TODO: assert truth table
    
  }

  std::cout << "All tests passed.\n";
  return 0;
}
