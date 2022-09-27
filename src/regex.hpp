#pragma once

#include <iostream>
#include <vector>
#include <string>

struct Regex;
enum class RegexType {
    Char, Concat, Sum, Star
};

struct CharRegex {
    char ch;

    explicit CharRegex(char ch) : ch(ch) {}
};

struct ConcatRegex {
    std::vector<Regex> operands{};
};

struct SumRegex {
    std::vector<Regex> operands{};
};

struct StarRegex {
    std::unique_ptr<Regex> operand{};

    explicit StarRegex(Regex &&contents);

    StarRegex(const StarRegex &copy);

    StarRegex(StarRegex &&move) noexcept;

    StarRegex &operator=(StarRegex &&move);

    StarRegex &operator=(const StarRegex &copy);

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
};

Regex operator ""_r(const char *string, size_t size);

std::ostream &operator<<(std::ostream &os, Regex const &regex);