#pragma once

#include "finite-automaton.hpp"

// Optimizes the automaton by removing unreachable states and deleting zero-transitions

class AutomatonOptimizer {
public:
    AutomatonOptimizer(FiniteAutomaton &automaton) : automaton(automaton) {

    }

    void remove_zero_transitions() {
        for(size_t i = 0; i < automaton.get_states().size(); i++) {
            for(size_t j = 0;; j++) {
                auto& transitions = automaton.get_states()[i].transitions;

                if(j >= transitions.size()) {
                    break;
                }

                if(transitions[j].regex.is_zero()) {
                    automaton.remove_transition(i, j);
                    j--;
                }
            }
        }
    }

    void remove_unreachable_states() {
        // Mark all states as unreachable
        std::vector<bool> reachable(automaton.get_states().size(), false);

        // Mark the initial state as reachable
        reachable[automaton.get_start_state_index()] = true;

        // Mark all states that can be reached from the initial state as reachable
        std::vector<size_t> current_states = {automaton.get_start_state_index()};

        while(!current_states.empty()) {
            size_t state_index = current_states.back();
            current_states.pop_back();

            for (auto &transition: automaton.get_states()[state_index].transitions) {
                if (!reachable[transition.target_index]) {
                    reachable[transition.target_index] = true;
                    current_states.push_back(transition.target_index);
                }
            }
        }

        // Remove all unreachable states
        for (int i = static_cast<int>(reachable.size()) - 1; i >= 0; i--) {
            if (!reachable[i]) {
                automaton.remove_state(i);
            }
        }
    }

    void optimize() {
        remove_zero_transitions();
        remove_unreachable_states();
    }

    FiniteAutomaton &automaton;
};