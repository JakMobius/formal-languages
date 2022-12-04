#pragma once

#include <string>
#include <unordered_set>
#include "grammar.hpp"

class CYKAlgorithm {
    const Grammar &grammar;
public:

    std::string word;
    std::vector<bool> table_vector{};
    std::vector<std::string> nonterminals{};
    int word_length = -1;
    int nonterminal_count = -1;

    CYKAlgorithm(const Grammar &grammar) : grammar(grammar) {
        nonterminals = get_nonterminals();
        nonterminal_count = nonterminals.size();
    }

    std::vector<std::string> get_nonterminals() {
        std::unordered_set<std::string> nonterminals;

        for (const auto &rule: grammar.get_rules()) {
            for (const auto &symbol: rule.left) {
                if (symbol.type == SymbolType::non_terminal) {
                    nonterminals.insert(std::get<NonTerminal>(symbol.value).id);
                }
            }
            for (const auto &symbol: rule.right) {
                if (symbol.type == SymbolType::non_terminal) {
                    nonterminals.insert(std::get<NonTerminal>(symbol.value).id);
                }
            }
        }

        return {nonterminals.begin(), nonterminals.end()};
    }

    std::vector<bool>::reference table(long i, long j, long k) {
        return table_vector[i * word_length * nonterminal_count + j * nonterminal_count + k];
    }

    int get_nonterminal_index(const std::string& nonterminal) {
        return std::find(nonterminals.begin(), nonterminals.end(), nonterminal) - nonterminals.begin();
    }

    int get_word_index(char c) {
        auto it = std::find(word.begin(), word.end(), c);
        if (it == word.end()) {
            return -1;
        }
        return it - word.begin();
    }

    void init_table() {
        for (const auto &rule: grammar.get_rules()) {
            if (rule.right.size() == 1 && rule.right[0].type == SymbolType::terminal) {
                char terminal = std::get<Terminal>(rule.right[0].value).c;

                auto word_index = get_word_index(terminal);

                if (word_index == -1) {
                    continue;
                }

                std::string nonterminal = std::get<NonTerminal>(rule.left[0].value).id;
                auto nonterminal_index = get_nonterminal_index(nonterminal);

                table(nonterminal_index, word_index, word_index) = true;
            }
        }
    }

    void table_iteration(int l) {
        for (int substr_start = 0; substr_start < word_length - l; substr_start++) {
            int substr_end = substr_start + l;
            for (int k = substr_start; k < substr_end; k++) {
                for (const auto &rule: grammar.get_rules()) {
                    if (rule.right.size() == 2) {
                        std::string nonterminal = std::get<NonTerminal>(rule.left[0].value).id;
                        std::string nonterminal1 = std::get<NonTerminal>(rule.right[0].value).id;
                        std::string nonterminal2 = std::get<NonTerminal>(rule.right[1].value).id;

                        auto nonterminal_index = get_nonterminal_index(nonterminal);
                        auto nonterminal1_index = get_nonterminal_index(nonterminal1);
                        auto nonterminal2_index = get_nonterminal_index(nonterminal2);

                        if(table(nonterminal1_index, substr_start, k) &&
                           table(nonterminal2_index, k + 1, substr_end)) {
                            table(nonterminal_index, substr_start, substr_end) = true;
                        }
                    }
                }
            }
        }
    }

    bool check(const std::string &input) {
        word = input;
        word_length = word.length();

        table_vector = std::vector(nonterminal_count * word_length * word_length, false);


        init_table();

        for (int l = 1; l < word_length; l++) {
            table_iteration(l);
        }

        int start_index = get_nonterminal_index(grammar.get_start_nonterminal().id);

        bool result = table(start_index, 0, word_length - 1);
        table_vector.clear();
        return result;
    }

};