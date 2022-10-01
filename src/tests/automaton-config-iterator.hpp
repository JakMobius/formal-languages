#pragma once

#include "gtest/gtest.h"
#include "../engine/finite-automaton.hpp"
#include "../engine/automaton-simplifier.hpp"
#include "../engine/epsilon-remover.hpp"
#include "../engine/automaton-optimizer.hpp"
#include "../engine/automaton-completer.hpp"
#include "../engine/automaton-determinator.hpp"

class AutomatonConfigIterator {
public:
    AutomatonConfigIterator(FiniteAutomaton &automaton) : automaton(automaton) {
        current_config = 0;
        next();
    }

    bool next() {
        current_config++;
        set_config();
        return current_config <= max_configs;
    }

    void set_config() {
        switch (current_config) {
            case 1:
                AutomatonSimplifier(automaton).simplify();
                EXPECT_TRUE(automaton.is_simple()) << "AutomatonSimplifier failed to make automaton simple";
                break;
            case 2:
                EpsilonRemover(automaton).simplify();
                EXPECT_TRUE(!automaton.has_epsilon_transitions()) << "EpsilonRemover failed to remove epsilon transitions";
                break;
            case 3:
                AutomatonOptimizer(automaton).optimize();
                break;
            case 4:
                AutomatonCompleter(automaton).complete();
                EXPECT_TRUE(automaton.is_complete()) << "AutomatonCompleter failed to complete the automaton";
                break;
            case 5:
                automaton = AutomatonDeterminator(automaton).determine();
                EXPECT_TRUE(automaton.is_deterministic()) << "AutomatonCompleter failed to create deterministic automaton";

                break;
            default: break;
        }
    }

    explicit operator bool() {
        return current_config <= max_configs;
    }

    FiniteAutomaton &automaton;

    size_t max_configs = 5;
    size_t current_config;
};