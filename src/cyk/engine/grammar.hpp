#pragma once

#include <vector>
#include <unordered_set>
#include "grammar-symbol.hpp"

struct GrammarRule {
    std::vector<GrammarSymbol> left;
    std::vector<GrammarSymbol> right;

    GrammarRule(): left(), right() {}
    GrammarRule(std::initializer_list<GrammarSymbol> left, std::initializer_list<GrammarSymbol> right) : left(left), right(right) {}

    template<typename T1, typename T2>
    GrammarRule(T1&& left, T2&& right) : left(std::forward<T1>(left)), right(std::forward<T2>(right)) {}

    bool operator==(const GrammarRule& other) const {
        return left == other.left && right == other.right;
    }

    bool operator!=(const GrammarRule& other) const {
        return !(*this == other);
    }

    bool uses_nonterminal_name(const std::string& name) const {
        for (const GrammarSymbol& symbol : left) {
            if(symbol == NonTerminal{name}) {
                return true;
            }
        }
        for (const GrammarSymbol& symbol : right) {
            if(symbol == NonTerminal{name}) {
                return true;
            }
        }
        return false;
    }
};

namespace std {
    template<>
    struct hash<GrammarRule> {
        std::size_t operator()(const GrammarRule& rule) const {
            std::size_t result = 0;
            for (const GrammarSymbol& symbol : rule.left) {
                result ^= std::hash<GrammarSymbol>()(symbol);
            }
            for (const GrammarSymbol& symbol : rule.right) {
                result ^= std::hash<GrammarSymbol>()(symbol);
            }
            return result;
        }
    };
}

class Grammar {
    std::vector<GrammarRule> rules;
    std::unordered_set<char> alphabet;

    NonTerminal start_nonterminal;
public:
    Grammar() = default;

    void add_rule(const GrammarRule& rule) {
        for(auto& symbol : rule.left) {
            if (symbol.type == SymbolType::terminal) {
                alphabet.insert(std::get<Terminal>(symbol.value).c);
            }
        }
        for(auto& symbol : rule.right) {
            if (symbol.type == SymbolType::terminal) {
                alphabet.insert(std::get<Terminal>(symbol.value).c);
            }
        }
        rules.push_back(rule);
    }

    void remove_rule(int index) {
        rules.erase(rules.begin() + index);
    }

    std::vector<GrammarRule>& get_rules() {
        return rules;
    }

    const std::vector<GrammarRule>& get_rules() const {
        return rules;
    }

    const std::unordered_set<char>& get_alphabet() const {
        return alphabet;
    }

    bool is_nonterminal_name_used(const std::string& name) const {
        for(const GrammarRule& rule : rules) {
            if(rule.uses_nonterminal_name(name)) {
                return true;
            }
        }
        return false;
    }

    NonTerminal get_start_nonterminal() const {
        return start_nonterminal;
    }

    void set_start_nonterminal(NonTerminal nonterminal) {
        start_nonterminal = nonterminal;
    }
};