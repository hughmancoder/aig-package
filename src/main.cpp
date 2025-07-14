#include <iostream>
#include <fstream>
#include "aig.hpp"

#include <iostream>
#include <fstream>
#include "aig.hpp"

int main() {
    {
        const std::string filename = "examples/and.aag";
        std::cout << "And: " << filename << "\n";

        std::ifstream f(filename);


        Aig aig;
        aig.parse(f);

        auto tt = aig.generate_truth_table();
        aig.display_truth_table(tt, aig.get_inputs().size());
    }

    
    {
        const std::string filename = "examples/full_adder.aag";
        std::cout << "Full Adder: " << filename << "\n";

        std::ifstream f(filename);

        Aig aig;
        aig.parse(f);

        auto tt = aig.generate_truth_table();
        aig.display_truth_table(tt, aig.get_inputs().size());
    }

    return 0;
}


/*     aig::Aig aig;
std::ifstream file("and.aag");

for (auto lit : aig.get_inputs())
    std::cout << "Input : " << lit << "\n";
for (auto lit : aig.get_outputs())
    std::cout << "Output: " << lit << "\n";

for (auto const &node : aig.get_nodes()) {
    std::cout << "Node " << node.id;
    if (node.type == aig::Aig::Type::AND)
        std::cout << " = AND(" << node.fanin0 << ", " << node.fanin1 << ")";
    else if (node.type == aig::Aig::Type::INPUT)
        std::cout << " (INPUT)";
    else if (node.type == aig::Aig::Type::OUTPUT)
        std::cout << " (OUTPUT drives " << node.fanin0 << ")";
    std::cout << "\n";
} 
    */