// Update the path below to the correct location of aig.hpp if needed
#include "../src/aig.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

// Global expected truth tables

std::vector<std::vector<bool>>
    FULL_ADDER_EXPECTED_TT = {
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
    assert(aig.is_topologically_sorted());

    // std::cout << "std::AIG depth: " << aig.compute_depth() << "\n";
    assert(aig.compute_depth() == 2);
    
    // auto fan = aig.compute_fanout_counts();
    // assert(fan[aig.get_inputs()[0]] == 1);
    // assert(fan[aig.get_inputs()[1]] == 1);

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
    assert(aig.is_topologically_sorted());
    assert(aig.compute_depth() == 5);

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

  // read and parse full_adder.blif file (extension)
  /*
  {
    std::ifstream in("examples/full_adder.blif");

    Aig aig;
    aig.parse(in);

    assert(aig.get_inputs().size() == 3);
    assert(aig.get_outputs().size() == 2);
    assert(aig.is_topologically_sorted());

    auto tt = aig.generate_truth_table();

    assert(tt == FULL_ADDER_EXPECTED_TT);
  }
  */


  // Commented out as it is redundant
  /*
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

    std::cout << "\n.aag format parsing expected\n";
  }
  */