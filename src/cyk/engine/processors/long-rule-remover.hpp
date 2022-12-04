#pragma once

#include "../grammar.hpp"

class LongRuleRemover {
    Grammar &grammar;

public:
    LongRuleRemover(Grammar &grammar) : grammar(grammar) {}

    // Assumption: grammar is context-free,
    // terminals are lower-case, non-terminals are upper-case
    void remove() {

        // S -> ABCD turns into S -> AA', A' -> BB', B' -> CD

        for (int i = 0; i < grammar.get_rules().size(); i++) {
            auto &rule = grammar.get_rules()[i];
            if (rule.right.size() <= 2) {
                continue;
            }

            // If there are no terminals, perform the transformation

            int terminal_count = 0;
            for (const auto &symbol: rule.right) {
                if (symbol.type == SymbolType::terminal) {
                    terminal_count++;
                }
            }

            if (terminal_count != 0) {
                continue;
            }

            std::string previous_nonterm = std::get<NonTerminal>(rule.right[0].value).id;
            std::string new_nonterm = free_nonterminal_name(grammar);

            auto new_right = std::vector<GrammarSymbol>{NonTerminal{previous_nonterm}, NonTerminal{new_nonterm}};

            previous_nonterm = new_nonterm;

            for (int j = 1; j < rule.right.size() - 1; j++) {
                std::string nonterminal_char = std::get<NonTerminal>(rule.right[j].value).id;

                new_nonterm = previous_nonterm;

                // Add the new rule
                grammar.add_rule(GrammarRule{{NonTerminal{previous_nonterm}},
                                             {NonTerminal{nonterminal_char}, NonTerminal{new_nonterm}}});

                if (j < rule.right.size() - 2) {
                    new_nonterm = free_nonterminal_name(grammar);
                    std::get<NonTerminal>(grammar.get_rules().back().right[1].value).id = new_nonterm;

                    previous_nonterm = new_nonterm;
                } else {
                    std::get<NonTerminal>(grammar.get_rules().back().right[1].value).id = std::get<NonTerminal>(
                            rule.right.back().value).id;
                }
            }

            rule.right = new_right;
        }
    }
};