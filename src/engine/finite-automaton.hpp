#pragma once

#include <set>
#include <string>
#include <map>
#include <utility>
#include "regex.hpp"

struct FiniteAutomatonTransition {
    Regex regex;
    size_t target_index;
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

    void expand_states_for_eps_transitions(std::set<size_t> &current_states) const;

    std::set<size_t> get_transitions_for_regex(const std::set<size_t> &current_states, std::string_view &input) const;

    bool accepts(std::string_view input) const;

    size_t add_state(bool is_final);

    template<typename T>
    void add_transition(size_t from, size_t to, T &&regex) {
        regex.fill_alphabet(alphabet);
        states[from].transitions.push_back(FiniteAutomatonTransition{std::forward<T>(regex), to});
    }

    const std::vector<FiniteAutomatonState> &get_states() const { return states; }

    size_t get_start_state_index() const { return start_state_index; }

    void remove_transition(size_t state_index, size_t transition_index);

    const FiniteAutomatonTransition &get_transition(size_t state_index, size_t transition_index) const;

    void make_state_final(size_t state_index, bool is_final);

    void remove_state(size_t i);

    std::set<char> alphabet;

    void extend_alphabet(const std::set<char> &alphabet);

    bool is_simple() const;
    bool is_complete() const;
    bool is_deterministic() const;
    bool has_epsilon_transitions() const;

    int find_transition(char c, size_t source_index) const;
    int count_transitions(char c, size_t source_index) const;
    int find_transition(char c, size_t source_index, size_t target_index) const;

private:
    std::vector<FiniteAutomatonState> states;
    size_t start_state_index = 0;
};