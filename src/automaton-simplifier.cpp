
#include "automaton-simplifier.hpp"


void AutomatonSimplifier::shorten_transition(int state_index, int transition_index) {
    auto* transition = &automaton.get_transition(state_index, transition_index);

    switch (transition->regex.type) {
        case RegexType::Char:
            assert(!"Char is not a long transition");
            return;
        case RegexType::Concat: {
            int last_index = state_index;

            for (int i = 1;; i++) {
                auto &operands = std::get<ConcatRegex>(transition->regex.value).operands;

                if(i >= operands.size()) {
                    break;
                }

                int next_index = automaton.add_node(false);
                automaton.add_transition(last_index, next_index, operands[i - 1]);
                last_index = next_index;

                // Update transition, since it might have moved in memory
                transition = &automaton.get_transition(state_index, transition_index);
            }

            auto &operands = std::get<ConcatRegex>(transition->regex.value).operands;
            automaton.add_transition(last_index, transition->target_index, operands[operands.size() - 1]);
            automaton.remove_transition(state_index, transition_index);

            break;
        }
        case RegexType::Sum: {

            for (int i = 0;; i++) {
                auto &operands = std::get<SumRegex>(transition->regex.value).operands;

                if(i >= operands.size()) {
                    break;
                }

                automaton.add_transition(state_index, transition->target_index, operands[i]);
                // Update transition, since it might have moved in memory
                transition = &automaton.get_transition(state_index, transition_index);
            }

            automaton.remove_transition(state_index, transition_index);
            break;
        }
        case RegexType::Star:
            auto &star_regex = std::get<StarRegex>(transition->regex.value);

            int target_index = transition->target_index;
            int fictive_start = automaton.add_node(false);
            int fictive_end = automaton.add_node(false);

            automaton.add_transition(fictive_start, fictive_end, star_regex.get_operand());
            automaton.add_transition(state_index, fictive_start, Regex());
            automaton.add_transition(fictive_end, state_index, Regex());
            automaton.add_transition(state_index, target_index, Regex());
            automaton.remove_transition(state_index, transition_index);

            break;
    }
}

bool AutomatonSimplifier::get_long_transition(int &state, int &transition_index) const {
    for (int i = 0; i < automaton.get_states().size(); i++) {
        auto &transitions = automaton.get_states()[i].transitions;

        for (int j = 0; j < transitions.size(); j++) {
            if (transitions[j].regex.type != RegexType::Char) {
                state = i;
                transition_index = j;
                return true;
            }
        }
    }
    return false;
}

void AutomatonSimplifier::remove_long_transitions() {
    int state = -1;
    int transition_index = -1;

    while (get_long_transition(state, transition_index)) {
        shorten_transition(state, transition_index);
    }
}