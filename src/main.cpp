#include <iostream>
#include <fstream>
#include "aig.hpp"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Usage:\n"
              << "  " << argv[0] << " stats  <path_to_file/file.aag>\n"
              << "  " << argv[0] << " truth  <path_to_file/file.aag>\n";
    return 1;
  }

  std::ifstream in(argv[2]);
  if (!in) { std::cerr << "Cannot open '" << argv[2] << "'\n"; return 1; }

  Aig aig; aig.parse(in);

  std::string cmd(argv[1]);
  if (cmd == "stats") {
    aig.print_stats();
  } else if (cmd == "truth_table") {
    auto tt = aig.generate_truth_table();
    std::cout << "Truth table (" << tt.size() << " rows):\n";
    aig.display_truth_table(tt, aig.get_inputs().size());
  } else {
    std::cerr << "Unknown command '" << cmd << "'\n";
    return 1;
  }
}


/*
int main() {    
    {
        const std::string filename = "examples/full_adder.aag";
        std::cout << "Full Adder: " << filename << "\n";

        std::ifstream f(filename);

        Aig aig;
        aig.parse(f);

        auto tt = aig.generate_truth_table();
        aig.display_truth_table(tt, aig.get_inputs().size());
    }

    {
        const std::string filename = "examples/and2.aag";
        std::cout << "and: " << filename << "\n";

        std::ifstream f(filename);


        Aig aig;
        aig.parse(f);

        auto tt = aig.generate_truth_table();
        aig.display_truth_table(tt, aig.get_inputs().size());
    }

    return 0;
}
*/
