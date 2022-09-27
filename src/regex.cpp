
#include "regex.hpp"


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
            for (auto &operand : std::get<ConcatRegex>(regex.value).operands) {
                os << operand;
            }
            break;
        case RegexType::Sum:
            os << "(";
            for (auto &operand : std::get<SumRegex>(regex.value).operands) {
                os << operand << "+";
            }
            os << "\b)";
            break;
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
    return {StarRegex(Regex(*this))};
}
