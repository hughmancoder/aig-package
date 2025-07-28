#include <iostream>
#include <fstream>
#include "aig.hpp"
#include "sweep.hpp"

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
    return 0;
  } else if (cmd == "truth_table") {
    auto tt = aig.generate_truth_table();
    std::cout << "Truth table (" << tt.size() << " rows):\n";
    aig.display_truth_table(tt, aig.get_inputs().size());
    return 0;
  } 
  else if (cmd == "write") {
    if (argc < 4) { std::cerr << "Missing <out.aag>\n"; return 1; }
    std::ofstream out(argv[3]);
    if (!out) { std::cerr << "Failed to write" << argv[3] << "\n"; return 1; }
    aig.write_aag(out);
  } else if (cmd == "sweep") {
    if (argc < 4) { std::cerr << "Missing <out.aag>\n"; return 1; }

    std::cout << "Before\n";
    aig.print_stats();

    auto reduced_aig = sweep_aig(aig);
    
    std::cout << "After\n";
    reduced_aig.print_stats();

    std::ofstream out(argv[3]);
    if (!out) { std::cerr << "Failed to open for write: " << argv[3] << "\n"; return 1; }
    reduced_aig.write_aag(out);
    return 0;
  }
  else {
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
