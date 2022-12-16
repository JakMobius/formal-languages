#pragma once

#include <unordered_set>
#include "../grammar.hpp"

class UselessSymbolRemover {
    Grammar& grammar;
public:


    UselessSymbolRemover(Grammar& grammar): grammar(grammar) {}

    bool fill_reachable(const std::vector<GrammarSymbol>& symbols, std::unordered_set<NonTerminal>& reachable) {
        bool changed = false;
        for (const auto& symbol : symbols) {
            if (symbol.type == SymbolType::non_terminal) {
                auto non_terminal = std::get<NonTerminal>(symbol.value);
                if (reachable.find(non_terminal) == reachable.end()) {
                    reachable.insert(non_terminal);
                    changed = true;
                }
            }
        }
        return changed;
    }

    std::unordered_set<NonTerminal> get_reachable_symbols(NonTerminal start) {
        std::unordered_set<NonTerminal> reachable_symbols {start};
        bool changed = true;
        while (changed) {
            changed = false;
            for (const auto& rule : grammar.get_rules()) {
                NonTerminal left = std::get<NonTerminal>(rule.left[0].value);
                if (reachable_symbols.find(left) != reachable_symbols.end()) {
                    changed |= fill_reachable(rule.right, reachable_symbols);
                }
            }
        }
        return reachable_symbols;
    }

    bool all_creative(const std::vector<GrammarSymbol>& symbols, const std::unordered_set<NonTerminal>& creative) {
        for (const auto& symbol : symbols) {
            if (symbol.type == SymbolType::non_terminal) {
                auto non_terminal = std::get<NonTerminal>(symbol.value);
                if (creative.find(non_terminal) == creative.end()) {
                    return false;
                }
            }
        }
        return true;
    }

    std::unordered_set<NonTerminal> get_creative_symbols() {
        std::unordered_set<NonTerminal> creative_symbols {};

        // Find rules that produce only terminals

        for (const auto& rule : grammar.get_rules()) {
            NonTerminal left = std::get<NonTerminal>(rule.left[0].value);
            bool is_creative = rule.right.size() == 1 && rule.right[0].type == SymbolType::terminal;
            if(is_creative) {
                creative_symbols.insert(left);
            }
        }

        // Extend the set while it is changing

        bool changed = true;

        while (changed) {
            changed = false;
            for (const auto& rule : grammar.get_rules()) {
                NonTerminal left = std::get<NonTerminal>(rule.left[0].value);
                if (creative_symbols.find(left) == creative_symbols.end()) {
                    if (all_creative(rule.right, creative_symbols)) {
                        changed = true;
                        creative_symbols.insert(left);
                    }
                }
            }
        }

        return creative_symbols;
    }

    // Assumption: grammar is context-free
    Grammar remove() {
        std::unordered_set<NonTerminal> reachable_symbols = get_reachable_symbols(grammar.get_start_nonterminal());
        std::unordered_set<NonTerminal> creative_symbols = get_creative_symbols();

        Grammar new_grammar;
        new_grammar.set_start_nonterminal(grammar.get_start_nonterminal());
        for (const auto& rule : grammar.get_rules()) {
            NonTerminal left = std::get<NonTerminal>(rule.left[0].value);
            if (reachable_symbols.find(left) == reachable_symbols.end()) {
                continue;
            }

            if (!all_creative(rule.right, creative_symbols)) {
                continue;
            }

            new_grammar.add_rule(rule);
        }
        return new_grammar;
    }
};