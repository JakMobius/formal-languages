
#include "automaton-simplifier.hpp"


void AutomatonSimplifier::shorten_transition(size_t state_index, size_t transition_index) {
    auto* transition = &automaton.get_transition(state_index, transition_index);

    switch (transition->regex.type) {
        case RegexType::Char:
            assert(!"Char is not a long transition");
            return;
        case RegexType::Concat: {
            size_t last_index = state_index;

            for (int i = 1;; i++) {
                auto &operands = std::get<ConcatRegex>(transition->regex.value).operands;

                if(i >= operands.size()) {
                    break;
                }

                size_t next_index = automaton.add_state(false);
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

            size_t target_index = transition->target_index;
            size_t fictive_start = automaton.add_state(false);
            size_t fictive_end = automaton.add_state(false);

            automaton.add_transition(fictive_start, fictive_end, star_regex.get_operand());
            automaton.add_transition(state_index, fictive_start, Regex());
            automaton.add_transition(fictive_end, fictive_start, Regex());
            automaton.add_transition(fictive_start, target_index, Regex());
            automaton.remove_transition(state_index, transition_index);

            break;
    }
}

bool AutomatonSimplifier::get_long_transition(size_t &state_index, size_t &transition_index) const {
    for (size_t i = 0; i < automaton.get_states().size(); i++) {
        auto &transitions = automaton.get_states()[i].transitions;

        for (size_t j = 0; j < transitions.size(); j++) {
            if (!CharRegex::is_char_transition(transitions[j].regex)) {
                state_index = i;
                transition_index = j;
                return true;
            }
        }
    }
    return false;
}

void AutomatonSimplifier::simplify() {
    size_t state = -1;
    size_t transition_index = -1;

    while (get_long_transition(state, transition_index)) {
        shorten_transition(state, transition_index);
    }
}