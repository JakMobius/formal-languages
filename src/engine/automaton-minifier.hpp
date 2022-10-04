#pragma once

#include <unordered_map>
#include "finite-automaton.hpp"

struct AutomatonMinifierTransition {
    char letter;
    int target;

    bool operator<(const AutomatonMinifierTransition& other) const {
        if(letter != other.letter) {
            return letter < other.letter;
        }
        return target < other.target;
    }
};

struct AutomatonMinifierEquivalenceClass {
    int class_index;
    std::vector<AutomatonMinifierTransition> transitions;

    bool operator<(const AutomatonMinifierEquivalenceClass& other) const {
        if(class_index != other.class_index) {
            return class_index < other.class_index;
        }
        return transitions < other.transitions;
    }
};

class AutomatonMinifier {
public:
    AutomatonMinifier(FiniteAutomaton &automaton) : automaton(automaton) {

    }

    FiniteAutomaton minify() {

        size_t state_count = automaton.get_states().size();

        std::vector<int> new_class_indices(state_count);
        std::vector<int> class_indices(state_count);
        std::map<AutomatonMinifierEquivalenceClass, std::set<int>> equiv_classes;

        for (int i = 0; i < state_count; i++) {
            class_indices[i] = automaton.get_states()[i].is_final ? 1 : 0;
        }

        while (class_indices != new_class_indices) {
            equiv_classes.clear();
            int max_class_index = 0;

            for (int state = 0; state < automaton.get_states().size(); state++) {
                AutomatonMinifierEquivalenceClass eq_class{ class_indices[state] };

                for (char letter : automaton.alphabet) {
                    int transitionIndex = automaton.find_transition(letter, state);
                    assert(transitionIndex != -1);

                    int targetClass = class_indices[automaton.get_transition(state, transitionIndex).target_index];

                    eq_class.transitions.push_back(AutomatonMinifierTransition { letter, targetClass });
                }

                equiv_classes[eq_class].insert(state);
            }

            for (auto& [eq_class, states] : equiv_classes) {
                for (int state : states) {
                    new_class_indices[state] = max_class_index;
                }

                max_class_index++;
            }

            std::swap(class_indices, new_class_indices);
        }

        FiniteAutomaton result;
        result.extend_alphabet(automaton.alphabet);

        std::vector<size_t> result_nodes(equiv_classes.size());
        for (auto& cls_node : result_nodes) {
            cls_node = result.add_state(false);
        }

        for (auto& [eq_class, states] : equiv_classes) {
            for (int old_state : states) {
                if (automaton.get_states()[old_state].is_final) {
                    result.make_state_final(eq_class.class_index, true);
                }

                if (automaton.get_start_state_index() == old_state) {
                    result.set_start_state(eq_class.class_index);
                }
            }

            for (auto& transition : eq_class.transitions) {
                result.add_transition(eq_class.class_index, transition.target, Regex(CharRegex(transition.letter)));
            }
        }

        return result;
    }

    FiniteAutomaton &automaton;
};