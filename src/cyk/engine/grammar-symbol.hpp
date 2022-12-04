#pragma once

#include <utility>
#include <variant>
#include <string>

struct Terminal {
    char c;

    Terminal(char c) : c(c) {}

    bool operator==(const Terminal& other) const {
        return c == other.c;
    }

    bool operator!=(const Terminal& other) const {
        return !(*this == other);
    }
};

struct NonTerminal {
    std::string id;

    NonTerminal(): id() {}
    NonTerminal(std::string id) : id(std::move(id)) {}

    bool operator==(const NonTerminal& other) const {
        return id == other.id;
    }

    bool operator!=(const NonTerminal& other) const {
        return !(*this == other);
    }
};

namespace std {
    template<>
    struct hash<Terminal> {
        size_t operator()(const Terminal& terminal) const {
            return std::hash<char>()(terminal.c);
        }
    };

    template<>
    struct hash<NonTerminal> {
        size_t operator()(const NonTerminal& non_terminal) const {
            return std::hash<std::string>()(non_terminal.id);
        }
    };
}

enum class SymbolType {
    terminal,
    non_terminal
};

struct GrammarSymbol {
    std::variant<Terminal, NonTerminal> value;
    SymbolType type;

    GrammarSymbol(Terminal terminal) : value(terminal), type(SymbolType::terminal) {}
    GrammarSymbol(NonTerminal non_terminal) : value(non_terminal), type(SymbolType::non_terminal) {}

    bool operator==(const GrammarSymbol& other) const {
        if(type != other.type) {
            return false;
        }

        if(type == SymbolType::terminal) {
            return std::get<Terminal>(value) == std::get<Terminal>(other.value);
        } else {
            return std::get<NonTerminal>(value) == std::get<NonTerminal>(other.value);
        }
    }

    bool operator!=(const GrammarSymbol& other) const {
        return !(*this == other);
    }
};