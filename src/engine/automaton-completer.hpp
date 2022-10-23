#pragma once

#include "finite-automaton.hpp"

// This class completes the automaton by inserting transitions to the trap state

class AutomatonCompleter {
public:
    AutomatonCompleter(FiniteAutomaton &automaton) : automaton(automaton) {}

    void complete() {
        size_t trap_state = automaton.add_state(false);
        auto& alphabet = automaton.alphabet;
        bool should_configure_trap = false;

        for (size_t i = 0; i < automaton.get_states().size(); i++) {
            if (i == trap_state) continue;

            for (char c : alphabet) {
                if (automaton.find_transition(c, i) == -1) {
                    automaton.add_transition(i, trap_state, Regex(CharRegex(c)));
                    should_configure_trap = true;
                }
            }
        }

        if(should_configure_trap) {
            for(char c : alphabet) {
                automaton.add_transition(trap_state, trap_state, Regex(CharRegex(c)));
            }
        } else {
            automaton.remove_state(trap_state);
        }

    }

    FiniteAutomaton &automaton;
};