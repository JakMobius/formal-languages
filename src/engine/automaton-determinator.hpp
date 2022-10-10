#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "finite-automaton.hpp"
#include "automaton-completer.hpp"

struct StateSuperposition {
    std::set<size_t> states;
    bool is_final = false;
    size_t id = 0;

    bool operator<(const StateSuperposition &other) const {
        if (states.size() < other.states.size()) {
            return true;
        }
        if (states.size() > other.states.size()) {
            return false;
        }
        if (is_final < other.is_final) {
            return true;
        }
        if (is_final > other.is_final) {
            return false;
        }
        return states < other.states;
    }
};

struct SuperpositionTransition {
    size_t source_index;
    size_t target_index;
    char ch;
};

struct SuperpositionVariant {
    char character;
    std::vector<size_t> targets;
};

struct StateBoundSuperposition {
    std::vector<SuperpositionVariant> transitions;

    void add_transition(char c, size_t state) {
        for (auto &variant: transitions) {
            if (variant.character == c) {
                variant.targets.push_back(state);
                return;
            }
        }
        transitions.push_back({c, {state}});
    }
};

// Creates DFA from simplified complete FA w/o epsilon-transitions.

struct AutomatonDeterminator {
    AutomatonDeterminator(const FiniteAutomaton &automaton) : automaton(automaton) {}

    void add_state_bound_superpositions() {
        auto &transitions = state_bound_superposition.transitions;

        for (auto &transition: transitions) {
            add_superposition_char_transition(transition.character, transition.targets);
        }
    }

    void add_superposition_char_transition(char c, const std::vector<size_t> &targets) {
        auto &superposition = next_superpositions[c];

        for (auto &target: targets) {
            superposition.states.insert(target);
            superposition.is_final = superposition.is_final || automaton.get_states()[target].is_final;
        }
    }

    bool find_new_superpositions() {
        bool changed = false;

        for (auto &state_superposition: found_superpositions) {
            for (auto &state_index: state_superposition.states) {
                state_bound_superposition.transitions.clear();

                auto &state = automaton.get_states()[state_index];

                for (auto &transition: state.transitions) {
                    if (CharRegex::is_char_transition(transition.regex)) {
                        char ch = CharRegex::get_char(transition.regex);
                        if (ch != '\0') {
                            state_bound_superposition.add_transition(ch, transition.target_index);
                        }
                    }
                }

                add_state_bound_superpositions();
            }

            for (auto &pair: next_superpositions) {
                char ch = pair.first;
                auto &superposition = pair.second;

                auto it = found_superpositions.find(superposition);
                if (it == found_superpositions.end()) {
                    StateSuperposition new_superposition = superposition;
                    new_superposition.id = found_superpositions.size();
                    found_transitions.push_back({state_superposition.id, new_superposition.id, ch});
                    found_superpositions.insert(std::move(new_superposition));
                    changed = true;
                } else {
                    found_transitions.push_back({state_superposition.id, it->id, ch});
                }
            }

            next_superpositions.clear();
        }

        return changed;
    }

    FiniteAutomaton determine() {
        assert(!automaton.has_epsilon_transitions());
        assert(automaton.is_complete());

        StateSuperposition start_superposition = {{automaton.get_start_state_index()}};
        start_superposition.is_final = automaton.get_states()[automaton.get_start_state_index()].is_final;
        found_superpositions = {std::move(start_superposition)};

        while (find_new_superpositions());

        FiniteAutomaton new_automaton;

        for (int i = 0; i < found_superpositions.size(); i++) {
            new_automaton.add_state(false);
        }

        for (auto &superposition: found_superpositions) {
            if (superposition.is_final) {
                new_automaton.make_state_final(superposition.id, true);
            }
        }

        for (auto &transition: found_transitions) {
            if (new_automaton.find_transition(transition.ch, transition.source_index, transition.target_index) == -1) {
                new_automaton.add_transition(transition.source_index, transition.target_index,
                                             Regex(CharRegex(transition.ch)));
            }
        }

        return new_automaton;
    }

    const FiniteAutomaton &automaton;

private:
    std::set<StateSuperposition> found_superpositions;
    std::vector<SuperpositionTransition> found_transitions;

    std::unordered_map<char, StateSuperposition> next_superpositions;
    StateBoundSuperposition state_bound_superposition;
};