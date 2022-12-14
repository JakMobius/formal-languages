#include <iostream>
#include "../engine/regex.hpp"
#include "../engine/finite-automaton.hpp"
#include "../engine/automaton-simplifier.hpp"
#include "../engine/epsilon-remover.hpp"
#include "../engine/automaton-optimizer.hpp"
#include "../engine/automaton-determinator.hpp"
#include "../engine/automaton-inverter.hpp"
#include "../engine/automaton-collapser.hpp"
#include "../engine/automaton-to-regex-converter.hpp"
#include "../engine/automaton-minifier.hpp"
#include "../engine/automaton-graphviz-printer.hpp"

Regex invert_regex(const Regex& regex, const std::set<char>& alphabet = {}) {
    FiniteAutomaton automaton(regex);
    automaton.extend_alphabet(alphabet);
    AutomatonSimplifier(automaton).simplify();

    EpsilonRemover(automaton).simplify();
    AutomatonOptimizer(automaton).optimize();
    AutomatonCompleter(automaton).complete();
    automaton = AutomatonDeterminator(automaton).determine();
    automaton = AutomatonMinifier(automaton).minify();
    AutomatonInverter(automaton).invert();

    std::cout << AutomatonGraphvizPrinter(automaton) << "\n";

    AutomatonCollapser(automaton).collapse();

    return AutomatonToRegexConverter(automaton).convert();
}

int main() {

//    Regex regex = *("ab"_r + "ba"_r) * (Regex() + "a"_r + "ba"_r);
//    Regex regex = *"ab"_r * *"b"_r + *(("a"_r + "b"_r) * ("a"_r + "b"_r));
//    Regex regex = "a"_r * *((*"ba"_r * *"a"_r * *"ab"_r) + "a"_r);

    Regex regex = "a"_r * *(*("ba"_r) * "a"_r * *("ab"_r) + "a"_r);

    std::cout << regex << "\n";

    std::cout << invert_regex(regex, {'a', 'b'}) << "\n";

    return 0;
}
