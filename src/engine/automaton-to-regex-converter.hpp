#pragma once

#include "finite-automaton.hpp"

// Converts collapsed automaton to a regex

struct AutomatonToRegexConverter {
    AutomatonToRegexConverter(const FiniteAutomaton &automaton) : automaton(automaton) {

    }

    Regex always_true_regex() {
        Regex regex;
        for(char c : automaton.alphabet) {
            regex += Regex(CharRegex(c));
        }

        return *regex;
    }

    Regex regex_for_states(size_t start_state, size_t end_state) {
        Regex start_loop_regex = Regex::empty();
        Regex end_loop_regex = Regex::empty();
        Regex forward_regex = Regex::zero();
        Regex backward_regex = Regex::zero();

        for(size_t i = 0; i < automaton.get_states()[start_state].transitions.size(); i++) {
            auto& transition = automaton.get_states()[start_state].transitions[i];
            if(transition.target_index == start_state) {
                start_loop_regex = transition.regex;
            } else if(transition.target_index == end_state) {
                forward_regex = transition.regex;
            }
        }

        for(size_t i = 0; i < automaton.get_states()[end_state].transitions.size(); i++) {
            auto& transition = automaton.get_states()[end_state].transitions[i];
            if(transition.target_index == end_state) {
                end_loop_regex = transition.regex;
            } else if(transition.target_index == start_state) {
                backward_regex = transition.regex;
            }
        }

        return *start_loop_regex * forward_regex * *(end_loop_regex + backward_regex * start_loop_regex * forward_regex);
    }

    Regex convert() {
        assert(automaton.get_states().size() <= 2);
        assert(!automaton.get_states().empty());

        bool first_final = automaton.get_states()[0].is_final;

        if(automaton.get_states().size() == 1) {
            return first_final ? always_true_regex() : Regex::empty();
        }

        bool second_final = automaton.get_states()[1].is_final;

        assert(first_final != second_final);
        assert(first_final != (automaton.get_start_state_index() == 0));

        return regex_for_states(first_final ? 1 : 0, first_final ? 0 : 1);
    }

    const FiniteAutomaton &automaton;
};