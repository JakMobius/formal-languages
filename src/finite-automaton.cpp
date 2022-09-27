
#include "finite-automaton.hpp"

FiniteAutomaton::FiniteAutomaton(const Regex &regex) {
    states.push_back(FiniteAutomatonState{false, {{regex, 1}}});
    states.push_back(FiniteAutomatonState{true, {}});
}

FiniteAutomaton::FiniteAutomaton(const FiniteAutomaton &copy) {
    *this = copy;
}

FiniteAutomaton::FiniteAutomaton(FiniteAutomaton &&move) noexcept {
    *this = std::move(move);
}

FiniteAutomaton &FiniteAutomaton::operator=(FiniteAutomaton &&move) {
    states = std::move(move.states);
    start_state_index = move.start_state_index;
    return *this;
}

FiniteAutomaton &FiniteAutomaton::operator=(const FiniteAutomaton &copy) {
    states = copy.states;
    start_state_index = copy.start_state_index;
    return *this;
}

void FiniteAutomaton::expand_states_for_eps_transitions(std::set<int> &current_states) const {
    bool changed = false;

    do {
        changed = false;
        for (auto state_index: current_states) {
            for (auto &transition: states[state_index].transitions) {
                if (transition.regex.type == RegexType::Char &&
                    std::get<CharRegex>(transition.regex.value).ch == '\0') {
                    if (current_states.contains(transition.target_index)) {
                        continue;
                    }
                    current_states.insert(transition.target_index);
                    changed = true;
                }
            }
        }
    } while (changed);
}

std::set<int>
FiniteAutomaton::get_transitions_for_regex(const std::set<int> &current_states, std::string_view &input) const {
    std::set<int> next_states;
    for (int state: current_states) {
        auto &transitions = states[state].transitions;

        for (auto &transition: transitions) {
            if (transition.regex.type == RegexType::Char) {
                auto &char_regex = std::get<CharRegex>(transition.regex.value);

                if (char_regex.ch == input[0]) {
                    next_states.insert(transition.target_index);
                }
            }
        }
    }

    if (!next_states.empty()) {
        input.remove_prefix(1);
    }

    return next_states;
}

bool FiniteAutomaton::accepts(std::string_view input) const {
    std::set<int> current_states = {start_state_index};
    expand_states_for_eps_transitions(current_states);

    while (!current_states.empty()) {
        if (input.empty()) {
            bool has_final_state = false;
            for (int state: current_states) {
                if (states[state].is_final) {
                    has_final_state = true;
                    break;
                }
            }

            return has_final_state;
        }

        current_states = get_transitions_for_regex(current_states, input);
        expand_states_for_eps_transitions(current_states);
    }

    return false;
}

int FiniteAutomaton::get_final_state_count() {
    int count = 0;
    for (auto &state: states) {
        if (state.is_final) {
            count++;
        }
    }
    return count;
}

void FiniteAutomaton::ensure_one_output() {
    int finalStateCount = get_final_state_count();
    if (finalStateCount <= 1) return;
    int finalStateIndex = static_cast<int>(states.size());
    states.emplace_back(FiniteAutomatonState{true, {}});

    for (auto &state: states) {
        if (!state.is_final) return;
        state.is_final = false;

        state.transitions.push_back(FiniteAutomatonTransition{Regex(), finalStateIndex});
    }
}

int FiniteAutomaton::add_node(bool is_final) {
    states.emplace_back(FiniteAutomatonState{is_final, {}});
    return static_cast<int>(states.size() - 1);
}

void FiniteAutomaton::remove_transition(int state_index, int transition_index) {
    states[state_index].transitions.erase(states[state_index].transitions.begin() + transition_index);
}

const FiniteAutomatonTransition &FiniteAutomaton::get_transition(int state_index, int transition_index) const {
    return states[state_index].transitions[transition_index];
}
