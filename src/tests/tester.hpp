#pragma once

#include "../engine/finite-automaton.hpp"

struct Tester {
    void assert_automaton_accepts(FiniteAutomaton& automaton, std::string_view input) {
        assert_true(automaton.accepts(input), "Automaton does not accept input");
    }

    void assert_automaton_rejects(FiniteAutomaton& automaton, std::string_view input) {
        assert_false(automaton.accepts(input), "Automaton does not reject input");
    }

    void assert_true(bool condition, const char *fail_message) {
        if(!condition) {
            throw std::runtime_error(fail_message);
        }
    }

    void assert_false(bool condition, const char *fail_message) {
        assert_true(!condition, fail_message);
    }

    void assert_has_substring(std::string_view str, std::string_view substr) {
        assert_true(str.find(substr) != std::string_view::npos, "String does not contain substring");
    }

    void assert_does_not_have_substring(std::string_view str, std::string_view substr) {
        assert_false(str.find(substr) != std::string_view::npos, "String contains substring");
    }
};