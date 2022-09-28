#pragma once

#include "finite-automaton.hpp"

class AutomatonInverter {
public:
    AutomatonInverter(FiniteAutomaton &automaton): automaton(automaton) {}

    void invert() {

        // Add all the transitions
        for (int state_index = 0; state_index < automaton.get_states().size(); state_index++) {
            auto &state = automaton.get_states()[state_index];

            automaton.make_state_final(state_index, !state.is_final);
        }
    }


    FiniteAutomaton &automaton;
};