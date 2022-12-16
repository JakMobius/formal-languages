#pragma once

#include <vector>
#include <set>

#include "../../cyk/engine/grammar.hpp"
#include "../../cyk/engine/grammar-symbol.hpp"
#include "../../cyk/engine/processors/free-nonterminal-name.hpp"

struct EarleyState {
    GrammarRule rule;
    int index_parent = 0;
    int dot_position = 0;

    bool operator==(const EarleyState& other) const;
};

namespace std {
    template<>
    struct hash<EarleyState> {
        size_t operator()(const EarleyState &rule) const {
            size_t hash = 0;
            for (auto &symbol: rule.rule.left) {
                hash = hash * 31 + std::hash<GrammarSymbol>()(symbol);
            }
            for (auto &symbol: rule.rule.right) {
                hash = hash * 31 + std::hash<GrammarSymbol>()(symbol);
            }
            hash = hash * 31 + std::hash<int>()(rule.index_parent);
            hash = hash * 31 + std::hash<int>()(rule.dot_position);
            return hash;
        }
    };
}

class EarleyParser {
public:

    EarleyParser(const Grammar &grammar) : grammar(grammar) {}

    bool parse(const std::string& input);

    std::unordered_set<EarleyState> scan(int index);

    std::unordered_set<EarleyState> predict(int index);

    std::unordered_set<EarleyState> complete(std::unordered_set<EarleyState> &new_situations);

    bool find_state(int i, const EarleyState& state) {
        return states[i].find(state) != states[i].end();
    }

private:

    std::vector<std::unordered_set<EarleyState>> states;
    std::string str;
    const Grammar &grammar;
};