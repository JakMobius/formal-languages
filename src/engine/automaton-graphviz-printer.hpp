#pragma once

#include "finite-automaton.hpp"

struct AutomatonGraphvizPrinter {
    AutomatonGraphvizPrinter(const FiniteAutomaton &automaton) : automaton(automaton) {}

    const FiniteAutomaton &automaton;
};

std::ostream &operator<<(std::ostream &stream, const AutomatonGraphvizPrinter &printer);