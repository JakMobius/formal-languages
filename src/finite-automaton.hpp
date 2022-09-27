#pragma once

#include <set>
#include <string>
#include <map>
#include <utility>
#include "regex.hpp"

struct FiniteAutomatonTransition {
    Regex regex;
    int target_index;
};

struct FiniteAutomatonState {
    bool is_final;
    std::vector<FiniteAutomatonTransition> transitions;
};

class FiniteAutomaton {
public:
    FiniteAutomaton() = default;

    FiniteAutomaton(const Regex &regex);

    FiniteAutomaton(const FiniteAutomaton &copy);

    FiniteAutomaton(FiniteAutomaton &&move) noexcept;

    FiniteAutomaton &operator=(FiniteAutomaton &&move);

    FiniteAutomaton &operator=(const FiniteAutomaton &copy);

    void expand_states_for_eps_transitions(std::set<int> &current_states) const;

    std::set<int> get_transitions_for_regex(const std::set<int> &current_states, std::string_view &input) const;

    bool accepts(std::string_view input) const;

    int get_final_state_count();
    void ensure_one_output();

    int add_node(bool is_final);

    template<typename T>
    void add_transition(int from, int to, T &&regex) {
        states[from].transitions.push_back(FiniteAutomatonTransition{std::forward<T>(regex), to});
    }

    const std::vector<FiniteAutomatonState> &get_states() const { return states; }

    int get_start_state_index() const { return start_state_index; }

    void remove_transition(int state_index, int transition_index);

    const FiniteAutomatonTransition &get_transition(int state_index, int transition_index) const;

private:
    std::vector<FiniteAutomatonState> states;
    int start_state_index = 0;
};