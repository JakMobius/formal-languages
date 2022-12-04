#pragma once

#include <ostream>
#include "grammar.hpp"

class GrammarPrinter {
public:
    GrammarPrinter(const Grammar& grammar) : grammar(grammar) {}

    void print_rules(std::ostream& output) const {
        for (const auto& rule : grammar.get_rules()) {
            print_rule(output, rule);
        }
    }

    void print_rule(std::ostream& output, const GrammarRule& rule) const {
        for (const auto& symbol : rule.left) {
            print_symbol(output, symbol);
        }
        output << " ::= ";
        for (const auto& symbol : rule.right) {
            print_symbol(output, symbol);
        }
        output << "\n";
    }

    void print_char(std::ostream& output, char ch) const {
        switch(ch) {
            case '<':
            case '>':
            case '\\':
            case '|':
                output << '\\' << ch;
                break;
            case '\n':
                output << "\\n";
                break;
            case '\r':
                output << "\\r";
                break;
            case '\t':
                output << "\\t";
                break;
            case ' ':
                output << "\\w";
                break;
            default:
                output << ch;
        }
    }

    void print_symbol(std::ostream& output, const GrammarSymbol& symbol) const {
        if (symbol.type == SymbolType::terminal) {
            print_char(output, std::get<Terminal>(symbol.value).c);
        } else {
            output << "<";

            for(auto ch : std::get<NonTerminal>(symbol.value).id) {
                print_char(output, ch);
            }

            output << ">";
        }
    }

    void print(std::ostream& output) const {
        print_rules(output);
    }

    const Grammar& grammar;
};

std::ostream& operator<<(std::ostream& os, const GrammarPrinter& printer);