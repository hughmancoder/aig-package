#include "aig.hpp"
#include <sstream>
#include <filesystem>

namespace {
bool is_whitespace_line(const std::string &s) {
    for(char c : s) if(!std::isspace(c)) return false;
    return true;
}

std::vector<std::string> split_ws(const std::string &s) {
    std::stringstream ss(s);
    std::vector<std::string> tok;
    std::string w;
    while (ss >> w) tok.push_back(w);
    return tok;
}

// Return Type::AND, Type::INPUT passthrough, etc.
// Currently only AND ("11 1") and BUF ("1 1") supported.
Aig::Type decode_truth_table(const std::vector<std::string>& tt,
                             bool &is_and_gate) {
    if (tt.size() == 2 && tt[0] == "11" && tt[1] == "1") {
        is_and_gate = true;              // 2‑input AND
        return Aig::Type::AND;
    }
    if (tt.size() == 2 && tt[0] == "1" && tt[1] == "1") {
        is_and_gate = false;             // simple buffer
        return Aig::Type::OUTPUT;        // treat as a wire
    }
    throw std::runtime_error("Unsupported truth table in .blif");
}
} // namespace
// -----------------------------------------------------------------

void Aig::parse_blif_and_construct(std::istream &in) {
    std::string line;
    std::unordered_map<std::string, Lit> name2lit;
    auto next_id = [&](){ return static_cast<Lit>(nodes_.size()*2); };

    // 1. Scan file
    while (std::getline(in, line)) {
        if (is_whitespace_line(line)) continue;

        auto tok = split_ws(line);
        if (tok[0] == ".model") { continue; }                   // ignore
        else if (tok[0] == ".inputs") {
            for (size_t i=1;i<tok.size();++i){
                Lit id = next_id();
                add_input_node(id);
                name2lit[tok[i]] = id;
            }
        }
        else if (tok[0] == ".outputs") {
            for (size_t i=1;i<tok.size();++i){
                // placeholder – we wire later
                name2lit[tok[i]] = 0;
            }
        }
        else if (tok[0] == ".names") {
            // .names a b y
            std::vector<std::string> signal_names(tok.begin()+1, tok.end());
            // truth‑table is on next non‑blank line
            std::string tt_line;
            while (std::getline(in, tt_line) && is_whitespace_line(tt_line));
            auto tt_tok = split_ws(tt_line);

            bool is_and=false;
            auto gate_type = decode_truth_table(tt_tok, is_and);
            if (!is_and) {         // buffer → connect output to input
                name2lit[signal_names.back()] = name2lit[signal_names.front()];
                continue;
            }
            // allocate literal for new AND node
            Lit node_lit = name2lit.contains(signal_names.back()) &&
                           name2lit[signal_names.back()]!=0 ?
                           name2lit[signal_names.back()] : next_id();

            add_and_gate(node_lit,
                          name2lit[signal_names[0]],
                          name2lit[signal_names[1]]);

            name2lit[signal_names.back()] = node_lit;
        }
        else if (tok[0] == ".end") break;
    }
    // 2. Wire up outputs now that everything has an id
    for (auto &[name, lit] : name2lit) {
        if (lit==0) throw std::runtime_error("Un‑driven output "+name);
    }
    for (auto &[name, lit] : name2lit) {
        if (lit && std::strncmp(name.c_str(), "out", 3)==0) add_output_node(lit);
    }
}

void Aig::write_blif(const std::filesystem::path &dst) const {
    std::ofstream out(dst);
    if(!out) throw std::runtime_error("Cannot open "+dst.string());
    out << ".model aig_export\n";

    // Inputs
    out << ".inputs";
    for(size_t i=0;i<inputs_.size();++i) out << " in" << i;
    out << "\n";

    // Outputs
    out << ".outputs";
    for(size_t i=0;i<outputs_.size();++i) out << " out" << i;
    out << "\n";

    auto lit_name = [&](Lit lit){
        for(size_t i=0;i<inputs_.size();++i) if(inputs_[i]==lit) return "in"+std::to_string(i);
        for(size_t i=0;i<outputs_.size();++i) if(outputs_[i]==lit) return "out"+std::to_string(i);
        return "n"+std::to_string(get_node_id(lit));
    };

    // AND gates
    for(const auto &n : nodes_){
        if(n.type!=Type::AND) continue;
        out << ".names " << lit_name(n.fanin0) << " " << lit_name(n.fanin1)
            << " "   << lit_name(n.literal) << "\n";
        out << "11 1\n";
    }

    // Tie each logical output to its driver (BUF)
    for(size_t i=0;i<outputs_.size();++i){
        Lit src = outputs_[i];
        out << ".names " << lit_name(src) << " out" << i << "\n"
            << "1 1\n";
    }
    out << ".end\n";
}
