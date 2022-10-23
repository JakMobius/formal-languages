#pragma once

#include "finite-automaton.hpp"

// Removes all complex transitions from the automaton
// Only leaves transitions with a single character or an epsilon

class AutomatonSimplifier {
public:
    AutomatonSimplifier(FiniteAutomaton &automaton) : automaton(automaton) {

    }

    bool get_long_transition(size_t &state_index, size_t &transition_index) const;

    void shorten_transition(size_t state_index, size_t transition_index);

    void simplify();

    FiniteAutomaton &automaton;
};