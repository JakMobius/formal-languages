#pragma once

// Removes unnecessary epsilon transitions from the automaton

#include "finite-automaton.hpp"

class EpsilonRemover {
public:
    EpsilonRemover(FiniteAutomaton &automaton) : automaton(automaton) {

    }

    void merge_states(std::vector<size_t> &states) {
        // Figure out whether there are any final states in the set
        bool is_final = false;
        for (auto state_index: states) {
            if (automaton.get_states()[state_index].is_final) {
                is_final = true;
                break;
            }
        }
        size_t merged = automaton.add_state(is_final);

        // Add transitions from the merged state to all the states in the set
        for (int state_index = 0; state_index < automaton.get_states().size(); state_index++) {
            auto &state = automaton.get_states()[state_index];
            bool input_merged = std::find(states.begin(), states.end(), state_index) != states.end();

            for (int i = 0;; i++) {
                auto& transitions = automaton.get_states()[state_index].transitions;

                if (i >= transitions.size()) {
                    break;
                }

                auto& transition = transitions[i];

                bool output_merged = std::find(states.begin(), states.end(), transition.target_index) != states.end();

                if (output_merged && !input_merged) {
                    automaton.add_transition(state_index, merged, transition.regex);
                } else if (input_merged && !output_merged) {
                    automaton.add_transition(merged, transition.target_index, transition.regex);
                } else if(input_merged && output_merged) {
                    if(transition.regex.type != RegexType::Char || std::get<CharRegex>(transition.regex.value).ch != 0) {
                        automaton.add_transition(merged, merged, transition.regex);
                    }
                }
            }
        }

        std::sort(states.begin(), states.end(), std::greater<>());

        // Remove all the states in the set
        for (auto state_index: states) {
            automaton.remove_state(state_index);
        }
    }

    bool remove_epsilon_loop_dfs(size_t state_index, std::vector<size_t>& stack, std::vector<bool> &stack_map) {
        stack_map[state_index] = true;
        stack.push_back(state_index);

        for(auto& transition : automaton.get_states()[state_index].transitions) {
            if (transition.regex.is_empty()) {
                if (stack_map[transition.target_index]) {
                    // Found a loop
                    std::vector<size_t> loop;

                    for (int i = stack.size() - 1; i >= 0; i--) {
                        loop.push_back(stack[i]);
                        if (stack[i] == transition.target_index) {
                            break;
                        }
                    }
                    merge_states(loop);
                    return true;
                } else if (remove_epsilon_loop_dfs(transition.target_index, stack, stack_map)) {
                    return true;
                }
            }
        }

        stack_map[state_index] = false;
        stack.pop_back();
        return false;
    }

    bool remove_epsilon_loop(size_t state_index) {
        // Search for epsilon loops with DFS

        std::vector<bool> visited(automaton.get_states().size(), false);
        std::vector<size_t> stack;
        return remove_epsilon_loop_dfs(state_index, stack, visited);
    }

    bool remove_epsilon_transition(size_t state_index, size_t transition_index) {

        // Duplicate all transitions from the target state to the current state

        size_t target_index = automaton.get_states()[state_index].transitions[transition_index].target_index;

        if (state_index == target_index) {
            automaton.remove_transition(state_index, transition_index);
            return true;
        }

        for (int i = 0;; i++) {
            auto &target = automaton.get_states()[target_index];
            auto &target_transitions = target.transitions;
            auto &target_transition = target_transitions[i];

            if (i >= target_transitions.size()) {
                break;
            }

            size_t target_transition_destination = target_transition.target_index;
            if (target_transition_destination == target_index) {
                target_transition_destination = state_index;
            }

            automaton.add_transition(state_index, target_transition_destination, target_transition.regex);
        }

        if (automaton.get_states()[target_index].is_final) {
            automaton.make_state_final(state_index, true);
        }

        automaton.remove_transition(state_index, transition_index);

        return true;
    }

    void simplify() {
        auto &states = automaton.get_states();
        for (size_t i = 0; i < states.size(); i++) {
            auto &transitions = states[i].transitions;
            for (size_t j = 0; j < transitions.size(); j++) {
                auto &regex = transitions[j].regex;
                if (regex.type == RegexType::Char && std::get<CharRegex>(regex.value).ch == '\0') {
                    if (remove_epsilon_loop(i)) {
                        i = -1;
                        break;
                    }
                    if (remove_epsilon_transition(i, j)) {
                        j--;
                    }
                }
            }
        }
    }

    FiniteAutomaton &automaton;
};