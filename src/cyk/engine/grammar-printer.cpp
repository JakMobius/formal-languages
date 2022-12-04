
#include "grammar-printer.hpp"

std::ostream &operator<<(std::ostream &os, const GrammarPrinter &printer) {
    printer.print(os);
    return os;
}
