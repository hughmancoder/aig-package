// Update the path below to the correct location of aig.hpp if needed
#include "../src/aig.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

// Global expected truth tables

std::vector<std::vector<bool>> FULL_ADDER_EXPECTED_TT = {
    // A B Cin | Cout Sum
    {0, 0, 0, 0, 0}, {0, 0, 1, 0, 1}, {0, 1, 0, 0, 1}, {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1}, {1, 0, 1, 1, 0}, {1, 1, 0, 1, 0}, {1, 1, 1, 1, 1}};

std::vector<std::vector<bool>> AND_EXPECTED_TT = {
    // A B | Y
    {0, 0, 0},
    {0, 1, 0},
    {1, 0, 0},
    {1, 1, 1}};

int main() {

  // Test enumerated inputs
  {
    Aig aig;
    auto inputs = aig.enumerate_inputs(2);

    // std::cout << "Test enumerated Inputs" << std::endl;
    // A B | Y
    std::vector<std::vector<bool>> expected = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    assert(inputs == expected);
  }

  // Test parse and2.aag file, aig construction and generate truth table
  {
    std::ifstream in("examples/and2.aag");
    assert(in && "unable to open examples/and.aag");
    Aig aig;
    aig.parse(in);

    assert(aig.get_inputs().size() == 2);
    assert(aig.get_outputs().size() == 1);
    assert(aig.get_nodes().size() == 4);

    std::cout << "AIG depth: " << aig.compute_depth() << "\n";
    assert(aig.compute_depth() == 1);

    
    size_t and_count = 0;
    for (auto const &n : aig.get_nodes())
      if (n.type == Aig::Type::AND)
        ++and_count;
    assert(and_count == 1);

    aig.print_stats();

    auto tt = aig.generate_truth_table();
    std::cout << "\nand2 aig truth table\n\n";
    std::cout << "A B | Y \n";
    aig.display_truth_table(tt, 2);
    std::cout << "\n";

    assert(tt == AND_EXPECTED_TT);
  }

  // Test read and parse full_adder.aag and generate truth table
  {
    std::ifstream in("examples/full_adder.aag");

    Aig aig;
    aig.parse(in);
    assert(aig.get_inputs().size() == 3);
    assert(aig.get_outputs().size() == 2);
    // assert(aig.compute_depth() == 4);

    /*  auto fan_out_counts = aig.compute_fanout_counts();
     for (const auto &[literal, count] : fan_out_counts) {
       std::cout << "Literal " << literal << " has fanout count: " << count
                 << std::endl;
     }
     std::size_t max_fanout = aig.get_max_fanout(fan_out_counts);
     assert(max_fanout == 4);
     */

    size_t and_count = 0;
    for (auto const &n : aig.get_nodes())
      if (n.type == Aig::Type::AND)
        ++and_count;
    assert(and_count == 11);

    aig.print_stats();

    auto tt = aig.generate_truth_table();
    std::cout << "\nfull_adder aig truth table:\n\n";
    std::cout << "A B Cin | Cout Sum \n";
    aig.display_truth_table(tt, 3);
    std::cout << "\n";

    assert(tt == FULL_ADDER_EXPECTED_TT);
  }

  std::cout << "All tests passed!\n";
  return 0;
}
