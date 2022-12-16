#pragma once

#include "../grammar.hpp"
#include "free-nonterminal-name.hpp"

class MixedRuleRemover {
    Grammar& grammar;

public:
    MixedRuleRemover(Grammar& grammar): grammar(grammar) {}

    // Assumption: grammar is context-free,
    // terminals are lower-case, non-terminals are upper-case
    void remove() {

        // S -> aBcDeF turns into S -> ABCDEF, A->a, C->c, E->e

        for(int i = 0; i < grammar.get_rules().size(); i++) {
            auto* rule = &grammar.get_rules()[i];
            if (rule->right.size() <= 2) {
                continue;
            }

            // If there are any nonterminals, perform the transformation

            int nonterminal_count = 0;
            for (const auto& symbol : rule->right) {
                if (symbol.type == SymbolType::non_terminal) {
                    nonterminal_count++;
                }
            }

            if (nonterminal_count == 0) {
                continue;
            }

            std::vector<GrammarSymbol> new_right;

            for (const auto& symbol : rule->right) {
                if (symbol.type == SymbolType::non_terminal) {
                    new_right.push_back(symbol);
                } else {
                    Terminal terminal = std::get<Terminal>(symbol.value);

                    std::string uppercase = {(char)toupper(terminal.c)};
                    NonTerminal new_non_terminal {free_nonterminal_name(grammar, uppercase)};

                    new_right.emplace_back(new_non_terminal);
                }
            }

            auto old_right = rule->right;

            for(const auto& symbol : old_right) {
                if (symbol.type == SymbolType::terminal) {
                    Terminal terminal = std::get<Terminal>(symbol.value);

                    std::string uppercase {(char)toupper(terminal.c)};
                    NonTerminal new_non_terminal {free_nonterminal_name(grammar, uppercase)};

                    grammar.add_rule(GrammarRule{{new_non_terminal}, {terminal}});
                }
            }

            grammar.get_rules()[i].right = new_right;
        }
    }
};