#pragma once

#include "finite-automaton.hpp"

struct DoubleTransition {
    size_t state_1_index;
    size_t transition_1_index;
    size_t state_2_index;
    size_t transition_2_index;
    size_t state_3_index;
};

// Collapses DFA into a single- or two-state FA

class AutomatonCollapser {
public:
    AutomatonCollapser(FiniteAutomaton &automaton) : automaton(automaton) {

    }

    size_t get_final_state_count() {
        size_t count = 0;
        for (auto &state: automaton.get_states()) {
            if (state.is_final) {
                count++;
            }
        }
        return count;
    }

    void ensure_one_output() {
        size_t final_state_count = get_final_state_count();
        if (final_state_count <= 1) return;
        size_t final_state_index = automaton.add_state(true);

        for (size_t i = 0; i < automaton.get_states().size(); i++) {
            if (!automaton.get_states()[i].is_final) continue;
            if (i == final_state_index) {
                continue;
            }
            automaton.make_state_final(i, false);

            automaton.add_transition(i, final_state_index, Regex());
        }
    }

    size_t pick_state_to_collapse() const {
        for (size_t i = 0; i < automaton.get_states().size(); i++) {
            auto &state = automaton.get_states()[i];
            if (state.is_final) continue;
            if (i == automaton.get_start_state_index()) continue;
            return i;
        }
        return -1;
    }

    void collapse_state(size_t state) {
        // Find double transitions that are going through the state

        std::vector<DoubleTransition> double_transitions;
        std::set<size_t> double_transition_starts;

        auto state_transitions = automaton.get_states()[state].transitions;

        Regex loop_regex;

        for (size_t state_transition = 0; state_transition < state_transitions.size(); state_transition++) {
            auto& transition = state_transitions[state_transition];

            if(transition.target_index == state) {
                loop_regex += transition.regex;
            }
        }

        for (size_t state_1_index = 0; state_1_index < automaton.get_states().size(); state_1_index++) {
            auto &state_1 = automaton.get_states()[state_1_index];

            for (size_t transition_1_index = 0; transition_1_index < state_1.transitions.size(); transition_1_index++) {
                auto &transition_1 = state_1.transitions[transition_1_index];
                if (transition_1.target_index != state) continue;

                for (size_t transition_2_index = 0;
                     transition_2_index < state_transitions.size(); transition_2_index++) {
                    auto& transition = state_transitions[transition_2_index];
                    size_t state_3_index = transition.target_index;

                    if(state_3_index == state) {
                        continue;
                    }

                    double_transition_starts.insert(state_1_index);

                    double_transitions.push_back(
                            {state_1_index, transition_1_index, state, transition_2_index, state_3_index});
                }
            }
        }

        if(!loop_regex.is_empty()) {
            loop_regex = *loop_regex;
        }

        // Add the double transitions
        for (auto &double_transition: double_transitions) {

            auto &state_1 = automaton.get_states()[double_transition.state_1_index];
            auto &state_2 = automaton.get_states()[double_transition.state_2_index];

            const Regex &regex_1 = state_1.transitions[double_transition.transition_1_index].regex;
            const Regex &regex_2 = state_2.transitions[double_transition.transition_2_index].regex;

            Regex regex = regex_1;
            if(!loop_regex.is_empty()) {
                regex *= loop_regex;
            }
            regex *= regex_2;

            automaton.add_transition(double_transition.state_1_index, double_transition.state_3_index,
                                     std::move(regex));
        }

        for(size_t start_state : double_transition_starts) {
            while(collapse_multiple_edges(start_state));
        }

        // Remove the state
        automaton.remove_state(state);
    }

    bool collapse_multiple_edges(size_t state_index) {
        std::vector<size_t> transitions_to_collapse;
        std::set<size_t> met_targets;
        bool found_multiple_edges = false;
        size_t transition_target_index = 0;

        auto &state = automaton.get_states()[state_index];
        if (state.transitions.size() <= 1) return false;

        transitions_to_collapse.clear();
        for (size_t j = 0; j < state.transitions.size(); j++) {
            size_t target_index = state.transitions[j].target_index;

            if(met_targets.contains(target_index)) {
                transition_target_index = target_index;
                found_multiple_edges = true;
            } else {
                met_targets.insert(target_index);
            }
        }

        if(!found_multiple_edges) return false;

        for (size_t j = 0; j < state.transitions.size(); j++) {
            size_t target_index = state.transitions[j].target_index;

            if(target_index == transition_target_index) {
                transitions_to_collapse.push_back(j);
            }
        }

        Regex regex = Regex::zero();

        for (size_t i : transitions_to_collapse) {
            regex += state.transitions[i].regex;
        }

        for(int j = static_cast<int>(transitions_to_collapse.size()) - 1; j >= 0; j--) {
            automaton.remove_transition(state_index, transitions_to_collapse[j]);
        }

        automaton.add_transition(state_index, transition_target_index, std::move(regex));

        return true;
    }

    void collapse() {
        assert(automaton.is_deterministic());
        ensure_one_output();

        for(size_t i = 0; i < automaton.get_states().size(); i++) {
            while(collapse_multiple_edges(i));
        }

        size_t state = -1;
        while ((state = pick_state_to_collapse()) != -1) {
            collapse_state(state);
        }
    }

    FiniteAutomaton &automaton;
};