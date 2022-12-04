#pragma once

#include <string>
#include <istream>
#include "grammar.hpp"

class GrammarParser {
    std::vector<char> alphabet;
public:
    GrammarParser() = default;

    Grammar parse(std::istream &input) {
        Grammar result;
        std::string line;

        while (std::getline(input, line)) {
            if (line.empty()) {
                continue;
            }
            parse_rule(line, result);
        }

        result.set_start_nonterminal(std::get<NonTerminal>(result.get_rules()[0].left[0].value));

        return result;
    }

    void parse_rule(const std::string &line, Grammar &grammar) {
        GrammarRule result;
        std::string left;
        std::vector<std::string> right_operands;
        std::string right;

        bool is_left = true;
        bool escape = false;

        for (char c: line) {
            if (!escape) {
                if (c == '\\') {
                    escape = true;
                    continue;
                }
                switch(c) {
                    case ' ':
                    case ':':
                        continue;
                    case '=':
                        is_left = false;
                        continue;
                    case '|':
                        right_operands.push_back(right);
                        right.clear();
                        continue;
                    default:
                        break;
                }
            } else {
                switch (c) {
                    case 't':
                        c = '\t';
                        break;
                    case 'n':
                        c = '\n';
                        break;
                    case 'r':
                        c = '\r';
                        break;
                    case ' ':
                    case 'w':
                        c = ' ';
                        break;
                    default:
                        if (is_left) {
                            left.push_back('\\');
                        } else {
                            right.push_back('\\');
                        }
                        break;
                }
                escape = false;
            }
            if (is_left) {
                left.push_back(c);
            } else {
                right.push_back(c);
            }
        }
        right_operands.push_back(right);
        auto left_symbols = parse_symbols(left);
        for (auto &right_operand: right_operands) {
            grammar.add_rule({left_symbols, parse_symbols(right_operand)});
        }
    }

    std::vector<GrammarSymbol> parse_symbols(const std::string &line) {
        std::vector<GrammarSymbol> result;
        bool is_nonterminal = false;
        bool is_escape = false;
        std::string nonterminal_name;
        for (char c: line) {
            if(!is_escape) {
                if (c == '\\') {
                    is_escape = true;
                    continue;
                }
                if (c == '<') {
                    is_nonterminal = true;
                    continue;
                }
                if (c == '>') {
                    is_nonterminal = false;
                    result.push_back(NonTerminal{nonterminal_name});
                    nonterminal_name.clear();
                    continue;
                }
            }
            is_escape = false;
            if (is_nonterminal) {
                nonterminal_name.push_back(c);
                continue;
            }
            result.push_back(Terminal{c});
        }
        return result;
    }
};