#pragma once

#include "../grammar.hpp"
#include "useless-symbol-remover.hpp"

class ChainRuleRemover {
    Grammar &grammar;
public:
    ChainRuleRemover(Grammar &grammar) : grammar(grammar) {}

    bool remove_iteration() {
        // Find for rule A -> B, and then replace all rules B -> (...) with A -> (...)

        for (int i = 0; i < grammar.get_rules().size(); i++) {
            const auto &rule = grammar.get_rules()[i];
            if (rule.right.size() == 1 && rule.right[0].type == SymbolType::non_terminal) {
                auto left = std::get<NonTerminal>(rule.left[0].value);
                auto right = std::get<NonTerminal>(rule.right[0].value);
                grammar.get_rules().erase(grammar.get_rules().begin() + i);

                for (int j = 0; j < grammar.get_rules().size(); j++) {
                    auto &rule_to_replace = grammar.get_rules()[j];
                    if (rule_to_replace.left[0] == right) {
                        grammar.add_rule(GrammarRule{std::vector<GrammarSymbol>{left}, rule_to_replace.right});
                    }
                }

                grammar = UselessSymbolRemover(grammar).remove();

                return true;
            }
        }

        return false;
    }

    // Assumption:
    // - grammar is context-free
    // - there are only rules of the form:
    //   - A -> a
    //   - A -> B
    //   - A -> BC
    //   - S -> eps
    void remove() {
        while (remove_iteration()) {}
    }
};