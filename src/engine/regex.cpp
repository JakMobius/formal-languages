
#include "regex.hpp"
#include <sstream>
#include <iostream>

Regex operator ""_r(const char *string, size_t size) {
    ConcatRegex result;

    for (size_t i = 0; i < size; i++) {
        result.operands.emplace_back(CharRegex(string[i]));
    }

    return {std::move(result)};
}

std::ostream &operator<<(std::ostream &os, Regex const &regex) {
    switch (regex.type) {
        case RegexType::Char: {
            char ch = std::get<CharRegex>(regex.value).ch;
            if (ch == '\0') {
                os << "ε";
            } else {
                os << ch;
            }
            break;
        }
        case RegexType::Concat:
            for (auto &operand: std::get<ConcatRegex>(regex.value).operands) {
                os << operand;
            }
            break;
        case RegexType::Sum: {
            os << "(";
            bool first = true;
            for (auto &operand: std::get<SumRegex>(regex.value).operands) {
                if (!first) {
                    os << "+";
                }
                os << operand;
                first = false;
            }
            os << ")";
            break;
        }
        case RegexType::Star:
            os << "(" << *std::get<StarRegex>(regex.value).operand << ")*";
            break;
    }
    return os;
}

StarRegex::StarRegex(Regex &&contents) {
    operand = std::make_unique<Regex>(std::move(contents));
}

StarRegex::StarRegex(const StarRegex &copy) {
    *this = copy;
}

StarRegex::StarRegex(StarRegex &&move) noexcept {
    *this = std::move(move);
}

StarRegex &StarRegex::operator=(StarRegex &&move) {
    operand = std::move(move.operand);
    return *this;
}

StarRegex &StarRegex::operator=(const StarRegex &copy) {
    operand = std::make_unique<Regex>(*copy.operand);
    return *this;
}

Regex &Regex::operator=(const Regex &copy) {
    type = copy.type;

    switch (type) {
        case RegexType::Char:
            value = CharRegex(std::get<CharRegex>(copy.value).ch);
            break;
        case RegexType::Concat:
            value = ConcatRegex(std::get<ConcatRegex>(copy.value));
            break;
        case RegexType::Sum:
            value = SumRegex(std::get<SumRegex>(copy.value));
            break;
        case RegexType::Star:
            value = StarRegex(std::get<StarRegex>(copy.value));
            break;
    }
    return *this;
}

Regex &Regex::operator=(Regex &&move) noexcept {
    type = move.type;

    switch (type) {
        case RegexType::Char:
            value = CharRegex(std::get<CharRegex>(std::move(move.value)).ch);
            break;
        case RegexType::Concat:
            value = ConcatRegex(std::get<ConcatRegex>(std::move(move.value)));
            break;
        case RegexType::Sum:
            value = SumRegex(std::get<SumRegex>(std::move(move.value)));
            break;
        case RegexType::Star:
            value = StarRegex(std::get<StarRegex>(std::move(move.value)));
            break;
    }

    return *this;
}

Regex::Regex(const Regex &copy) {
    *this = copy;
}

Regex::Regex(Regex &&move) {
    *this = std::move(move);
}

Regex &Regex::operator+=(const Regex &right) {
    if(this->is_zero()) {
        *this = right;
        return *this;
    }

    if(right.is_zero()) {
        return *this;
    }

    if (type == RegexType::Sum) {
        std::get<SumRegex>(value).operands.push_back(right);
    } else {
        SumRegex concat{};
        concat.operands.push_back(*this);
        concat.operands.push_back(right);
        *this = Regex(std::move(concat));
    }
    return *this;
}

Regex &Regex::operator*=(const Regex &right) {
    if(this->is_zero()) {
        return *this;
    }

    if(right.is_zero()) {
        *this = Regex::zero();
        return *this;
    }

    if (this->is_empty()) {
        *this = right;
        return *this;
    }

    if (right.is_empty()) {
        return *this;
    }

    if (type == RegexType::Concat) {
        std::get<ConcatRegex>(value).operands.push_back(right);
    } else {
        ConcatRegex concat{};
        concat.operands.push_back(*this);
        concat.operands.push_back(right);
        *this = Regex(std::move(concat));
    }
    return *this;
}

Regex Regex::operator+(const Regex &right) const {
    Regex copy = *this;
    copy += right;
    return copy;
}

Regex Regex::operator*(const Regex &right) const {
    Regex copy = *this;
    copy *= right;
    return copy;
}

Regex Regex::operator*() {
    if (this->is_empty() || this->is_zero()) {
        return *this;
    }
    return {StarRegex(Regex(*this))};
}

void Regex::fill_alphabet(std::set<char> &alphabet) const {
    switch (type) {
        case RegexType::Char: {
            char c = std::get<CharRegex>(value).ch;
            if (c != '\0') {
                alphabet.insert(c);
            }
            break;
        }
        case RegexType::Concat:
            for (auto &operand: std::get<ConcatRegex>(value).operands) {
                operand.fill_alphabet(alphabet);
            }
            break;
        case RegexType::Sum:
            for (auto &operand: std::get<SumRegex>(value).operands) {
                operand.fill_alphabet(alphabet);
            }
            break;
        case RegexType::Star:
            std::get<StarRegex>(value).operand->fill_alphabet(alphabet);
            break;
    }
}

std::string Regex::print() const {
    std::stringstream ss;
    ss << (*this);
    auto str = ss.str();
    std::cout << str << "\n";
    return str;
}

bool Regex::is_zero() const {
    return type == RegexType::Sum && std::get<SumRegex>(value).operands.empty();
}

bool Regex::is_empty() const {
    return type == RegexType::Char && std::get<CharRegex>(value).ch == '\0';
}

char CharRegex::get_char(const Regex &regex) {
    return std::get<CharRegex>(regex.value).ch;
}

bool CharRegex::is_char_transition(const Regex &regex) {
    return regex.type == RegexType::Char;
}
