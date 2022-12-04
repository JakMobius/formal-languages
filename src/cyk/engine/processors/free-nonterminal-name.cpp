
#include "free-nonterminal-name.hpp"
#include "../grammar.hpp"

std::string free_nonterminal_name(const Grammar& grammar, const std::string& start_name) {
    if(!grammar.is_nonterminal_name_used(start_name)) {
        return start_name;
    }

    std::string result;
    int i = 1;
    while (true) {
        result = start_name + "_" + std::to_string(i);
        if(!grammar.is_nonterminal_name_used(result)) {
            return result;
        }
        i++;
    }
}
