#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <memory>
#include <variant>
#include <cassert>

struct Regex;
enum class RegexType {
    Char, Concat, Sum, Star
};

struct CharRegex {
    char ch;

    explicit CharRegex(char ch) : ch(ch) {}

    static bool is_char_transition(const Regex& regex);
    static char get_char(const Regex& regex);
};

struct ConcatRegex {
    std::vector<Regex> operands{};

    bool operator==(const ConcatRegex& other) const {
        return operands == other.operands;
    }
};

struct SumRegex {
    std::vector<Regex> operands{};

    bool operator==(const SumRegex& other) const {
        return operands == other.operands;
    }
};

struct StarRegex {
    std::unique_ptr<Regex> operand{};

    explicit StarRegex(Regex &&contents);

    StarRegex(const StarRegex &copy);

    StarRegex(StarRegex &&move) noexcept;

    StarRegex &operator=(StarRegex &&move);

    StarRegex &operator=(const StarRegex &copy);

    bool operator==(const StarRegex& other) const;

    const Regex &get_operand() const { return *operand.get(); }

    Regex &get_operand() { return *operand.get(); }
};

struct Regex {
    RegexType type = RegexType::Char;
    std::variant<CharRegex, ConcatRegex, SumRegex, StarRegex> value = CharRegex('\0');

    Regex &operator=(const Regex &copy);

    Regex &operator=(Regex &&move) noexcept;

    Regex() = default;

    ~Regex() {
        std::cout << "";
    }

    Regex(const Regex &copy);

    Regex(Regex &&move);

    Regex(const CharRegex &value) : type(RegexType::Char), value(value) {}

    Regex(ConcatRegex &&value) : type(RegexType::Concat), value(std::move(value)) {}

    Regex(const ConcatRegex &value) : type(RegexType::Concat), value(value) {}

    Regex(SumRegex &&value) : type(RegexType::Sum), value(std::move(value)) {}

    Regex(const SumRegex &value) : type(RegexType::Sum), value(value) {}

    Regex(StarRegex &&value) : type(RegexType::Star), value(std::move(value)) {}

    Regex(const StarRegex &value) : type(RegexType::Star), value(value) {}

    Regex &operator+=(const Regex &right);

    Regex &operator*=(const Regex &right);

    Regex operator+(const Regex &right) const;

    Regex operator*(const Regex &right) const;

    Regex operator*();

    Regex operator+();

    bool operator==(const Regex &other) const;

    void fill_alphabet(std::set<char>& alphabet) const;

    // To use in LLDB
    std::string print() const;

    static Regex zero() { return SumRegex(); }
    static Regex empty() { return {};}

    bool is_zero() const;
    bool is_empty() const;
};

Regex operator ""_r(const char *string, size_t size);

std::ostream &operator<<(std::ostream &os, Regex const &regex);