#include <iostream>
#include "regex.hpp"
#include "finite-automaton.hpp"
#include "automaton-graphviz-printer.hpp"
#include "automaton-simplifier.hpp"

int main() {

    Regex regex = *"a"_r * *("ab"_r + "cd"_r) * "ef"_r;
    std::cout << regex << "\n";

    FiniteAutomaton automaton(regex);

    AutomatonSimplifier::simplify(automaton);

    std::cout << AutomatonGraphvizPrinter(automaton) << "\n";

    std::cout << "abef: " << automaton.accepts("abef") << "\n";
    std::cout << "cdef: " << automaton.accepts("cdef") << "\n";
    std::cout << "abababef: " << automaton.accepts("abababef") << "\n";
    std::cout << "abcdabef: " << automaton.accepts("abcdabef") << "\n";
    std::cout << "aaaabcdabef: " << automaton.accepts("aaaabcdabef") << "\n";

    std::cout << "ab: " << automaton.accepts("ab") << "\n";
    std::cout << "cd: " << automaton.accepts("cd") << "\n";
    std::cout << "ababab: " << automaton.accepts("ababab") << "\n";
    std::cout << "abcdab: " << automaton.accepts("abcdab") << "\n";

    return 0;
}
