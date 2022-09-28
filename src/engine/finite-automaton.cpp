
#include "finite-automaton.hpp"

FiniteAutomaton::FiniteAutomaton(const Regex &regex) {
    states.push_back(FiniteAutomatonState{false, {{regex, 1}}});
    states.push_back(FiniteAutomatonState{true, {}});
    regex.fill_alphabet(alphabet);
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

void FiniteAutomaton::expand_states_for_eps_transitions(std::set<size_t> &current_states) const {
    bool changed = false;

    do {
        changed = false;
        for (auto state_index: current_states) {
            for (auto &transition: states[state_index].transitions) {
                if (CharRegex::is_char_transition(transition.regex) &&
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

std::set<size_t>
FiniteAutomaton::get_transitions_for_regex(const std::set<size_t> &current_states, std::string_view &input) const {
    std::set<size_t> next_states;
    for (int state: current_states) {
        auto &transitions = states[state].transitions;

        for (auto &transition: transitions) {
            if (CharRegex::is_char_transition(transition.regex)) {
                if (CharRegex::get_char(transition.regex) == input[0]) {
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
    assert(is_simple());

    std::set<size_t> current_states = {start_state_index};
    expand_states_for_eps_transitions(current_states);

    while (!current_states.empty()) {
        if (input.empty()) {
            bool has_final_state = false;
            for (size_t state: current_states) {
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

size_t FiniteAutomaton::add_state(bool is_final) {
    states.emplace_back(FiniteAutomatonState{is_final, {}});
    return states.size() - 1;
}

void FiniteAutomaton::remove_transition(size_t state_index, size_t transition_index) {
    states[state_index].transitions.erase(states[state_index].transitions.begin() + transition_index);
}

const FiniteAutomatonTransition &FiniteAutomaton::get_transition(size_t state_index, size_t transition_index) const {
    return states[state_index].transitions[transition_index];
}

void FiniteAutomaton::make_state_final(size_t state_index, bool is_final) {
    states[state_index].is_final = is_final;
}

void FiniteAutomaton::remove_state(size_t state_index) {
    // Remove state and all transitions connected to it

    for (int i = 0; i < states.size(); i++) {
        auto &state = states[i];
        for (int j = 0; j < state.transitions.size(); j++) {
            size_t target_index = state.transitions[j].target_index;
            if (target_index == state_index) {
                remove_transition(i, j);
                j--;
            } else if (target_index > state_index) {
                state.transitions[j].target_index--;
            }
        }
    }

    states.erase(states.begin() + state_index);
}

void FiniteAutomaton::extend_alphabet(const std::set<char> &other_alphabet) {
    for (char ch: other_alphabet) {
        alphabet.insert(ch);
    }
}

bool FiniteAutomaton::is_simple() const {
    for (auto &state: states) {
        for(int i = 0; i < state.transitions.size(); i++) {
            if(!CharRegex::is_char_transition(state.transitions[i].regex)) {
                return false;
            }
        }
    }

    return true;
}

bool FiniteAutomaton::is_complete() const {
    if(!is_simple()) {
        return false;
    }

    for (size_t i = 0; i < get_states().size(); i++) {
        for (char c: alphabet) {
            if (find_transition(c, i) == -1) {
                return false;
            }
        }
    }

    return true;
}

bool FiniteAutomaton::is_deterministic() const {
    if(!is_simple()) {
        return false;
    }

    for (size_t i = 0; i < get_states().size(); i++) {
        for (char c: alphabet) {
            if (count_transitions(c, i) > 1) {
                return false;
            }
        }
    }

    return true;
}

bool FiniteAutomaton::has_epsilon_transitions() const {
    for (auto &state: states) {
        for (auto &transition: state.transitions) {
            if (transition.regex.is_empty()) {
                return true;
            }
        }
    }

    return false;
}

int FiniteAutomaton::find_transition(char c, size_t source_index) const {
    auto &transitions = get_states()[source_index].transitions;

    for (int i = 0; i < transitions.size(); i++) {
        auto &transition = transitions[i];
        if (CharRegex::is_char_transition(transition.regex) &&
            std::get<CharRegex>(transition.regex.value).ch == c) {
            return i;
        }
    }

    return -1;
}

int FiniteAutomaton::count_transitions(char c, size_t source_index) const {
    auto &transitions = get_states()[source_index].transitions;
    int result = 0;

    for (int i = 0; i < transitions.size(); i++) {
        auto &transition = transitions[i];
        if (CharRegex::is_char_transition(transition.regex) &&
            std::get<CharRegex>(transition.regex.value).ch == c) {
            result++;
        }
    }

    return result;
}

int FiniteAutomaton::find_transition(char c, size_t source_index, size_t target_index) const {
    auto &transitions = get_states()[source_index].transitions;

    for (int i = 0; i < transitions.size(); i++) {
        auto &transition = transitions[i];
        if (transition.target_index == target_index &&
            CharRegex::is_char_transition(transition.regex) &&
            std::get<CharRegex>(transition.regex.value).ch == c) {
            return i;
        }
    }

    return -1;
}
