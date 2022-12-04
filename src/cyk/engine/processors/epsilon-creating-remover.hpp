#pragma once

#include "../grammar.hpp"
#include "free-nonterminal-name.hpp"

class EpsilonCreatingRemover {
    Grammar& grammar;
public:
    EpsilonCreatingRemover(Grammar& grammar) : grammar(grammar) {}

    bool remove_iteration() {
        std::vector<NonTerminal> epsilon_nonterminals;

        for (int i = 0; i < grammar.get_rules().size(); i++) {
            const GrammarRule& rule = grammar.get_rules()[i];

            if(rule.left[0] == grammar.get_start_nonterminal()) {
                continue;
            }

            if (rule.right.empty()) {
                epsilon_nonterminals.push_back(std::get<NonTerminal>(rule.left[0].value));
                grammar.get_rules().erase(grammar.get_rules().begin() + i);
                i--;
            }
        }

        if(epsilon_nonterminals.empty()) {
            return false;
        }

        for(int i = 0; i < grammar.get_rules().size(); i++) {
            auto& rule = grammar.get_rules()[i];
            if(rule.right.size() < 2) {
                continue;
            }

            // Check if the right part of the rule contains an epsilon-creating rule

            for(int j = 0; j < rule.right.size(); j++) {
                int epsilon_index = -1;
                const auto& symbol = rule.right[j];
                if(symbol.type == SymbolType::terminal) {
                    continue;
                }

                NonTerminal non_terminal = std::get<NonTerminal>(symbol.value);

                for(int k = 0; k < epsilon_nonterminals.size(); k++) {
                    if(non_terminal == epsilon_nonterminals[k]) {
                        epsilon_index = k;
                        break;
                    }
                }

                if(epsilon_index != -1) {
                    // Add new rule to the grammar
                    grammar.add_rule({rule.left, std::vector<GrammarSymbol>{rule.right[1 - epsilon_index]}});
                }
            }
        }

        return true;
    }

    bool epsilon_word_accepted() {
        for(const auto& rule : grammar.get_rules()) {
            if(rule.left[0] == grammar.get_start_nonterminal() && rule.right.empty()) {
                return true;
            }
        }

        return false;
    }

    // Assumption:
    // - grammar is context-free
    // - terminals are lower-case
    // - non-terminals are upper-case
    // - there are only rules of the form:
    //   - A -> a
    //   - A -> B
    //   - A -> BC
    //   - A -> eps
    void remove() {
        while(remove_iteration()) {}

        if(epsilon_word_accepted()) {
            // Make a new nonterminal S' and add a rule S' -> S
            NonTerminal new_nonterminal {free_nonterminal_name(grammar, grammar.get_start_nonterminal().id)};
            grammar.get_rules().push_back(GrammarRule{{new_nonterminal}, {grammar.get_start_nonterminal()}});

            // Add rule S' -> eps
            grammar.add_rule(GrammarRule{{new_nonterminal}, {}});

            grammar.set_start_nonterminal(new_nonterminal);

            // Remove rule S -> eps
            while(remove_iteration()) {}
        }
    }
};