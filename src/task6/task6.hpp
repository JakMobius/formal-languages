#pragma once

#include <string>
#include <vector>
#include <stack>
#include "../engine/regex.hpp"
#include "../engine/finite-automaton.hpp"
#include "../engine/automaton-simplifier.hpp"
#include "../engine/epsilon-remover.hpp"
#include "../engine/automaton-completer.hpp"
#include "../engine/automaton-minifier.hpp"
#include "../engine/automaton-determinator.hpp"

namespace Task6 {

    Regex parse_regex_invert_polish_notation(const std::string &regex) {
        std::stack <Regex> stack;
        for (char i: regex) {
            if (i == 'a' || i == 'b' || i == 'c') {
                stack.push(Regex(CharRegex(i)));
            } else if (i == '*') {
                Regex r = stack.top();
                stack.pop();
                stack.push(*r);
            } else if (i == '+') {
                Regex r1 = stack.top();
                stack.pop();
                Regex r2 = stack.top();
                stack.pop();
                stack.push(r1 + r2);
            } else if (i == '1') {
                stack.push(Regex());
            } else if (i == '.') {
                Regex r1 = stack.top();
                stack.pop();
                Regex r2 = stack.top();
                stack.pop();
                stack.push(r2 * r1);
            }
        }
        return stack.top();
    }

    int reach_final_state(FiniteAutomaton &automaton, size_t start_index) {
        if (automaton.get_states()[start_index].is_final) {
            return 0;
        }

        std::set <size_t> current_states;
        current_states.insert(start_index);
        int steps = 0;

        while (true) {
            steps++;
            std::set <size_t> next_states;
            for (auto state_index: current_states) {
                auto &state = automaton.get_states()[state_index];
                for (auto &transition: state.transitions) {
                    if (automaton.get_states()[transition.target_index].is_final) {
                        return steps;
                    }
                    next_states.insert(transition.target_index);
                }
            }
            size_t old_states_size = current_states.size();
            current_states.insert(next_states.begin(), next_states.end());
            if (current_states.size() == old_states_size) {
                break;
            }
        };

        return -1;
    }

    Regex inverse_regex(const Regex &regex) {
        switch (regex.type) {
            case RegexType::Char:
            case RegexType::Star:
            case RegexType::Sum:
                return regex;
            case RegexType::Concat: {
                auto &value = std::get<ConcatRegex>(regex.value);
                ConcatRegex reversed_regex;
                for (int i = value.operands.size() - 1; i >= 0; i--) {
                    reversed_regex.operands.push_back(inverse_regex(value.operands[i]));
                }
                return reversed_regex;
            }
        }
    }

    int solve(const std::string &input_string, char ch, int k) {
        Regex regex = inverse_regex(parse_regex_invert_polish_notation(input_string));

        FiniteAutomaton automaton(regex);
        automaton.extend_alphabet({'a', 'b', 'c'});
        AutomatonSimplifier(automaton).simplify();
        EpsilonRemover(automaton).simplify();
        AutomatonCompleter(automaton).complete();
        automaton = AutomatonDeterminator(automaton).determine();

        size_t current_state = automaton.get_start_state_index();

        int word_size = 0;

        for (int i = 0; i < k; i++) {
            int transition = automaton.find_transition(ch, current_state);
            current_state = automaton.get_transition(current_state, transition).target_index;
            word_size++;
        }

        // Determine whether the final state is reachable from the current state

        int steps = reach_final_state(automaton, current_state);

        if (steps == -1) {
            return -1;
        }

        return steps + word_size;
    }

};