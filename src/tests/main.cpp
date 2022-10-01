
#include "gtest/gtest.h"
#include "../engine/regex.hpp"
#include "../engine/finite-automaton.hpp"
#include "../engine/automaton-simplifier.hpp"
#include "automaton-config-iterator.hpp"
#include "../engine/automaton-graphviz-printer.hpp"

TEST(test_regex, test_regex_1) {
    Regex regex = *("a"_r);

    FiniteAutomaton automaton(regex);

    for(AutomatonConfigIterator config_iterator(automaton); config_iterator; config_iterator.next()) {
        EXPECT_TRUE(automaton.accepts(""));
        EXPECT_TRUE(automaton.accepts("a"));
        EXPECT_FALSE(automaton.accepts("b"));
        EXPECT_TRUE(automaton.accepts("aa"));
        EXPECT_FALSE(automaton.accepts("ab"));
        EXPECT_FALSE(automaton.accepts("ba"));
        EXPECT_FALSE(automaton.accepts("bb"));
        EXPECT_TRUE(automaton.accepts("aaa"));
    }
}

TEST(test_regex, test_regex_2) {
    Regex regex = "a"_r + "b"_r;

    FiniteAutomaton automaton(regex);

    for(AutomatonConfigIterator config_iterator(automaton); config_iterator; config_iterator.next()) {
        EXPECT_FALSE(automaton.accepts(""));
        EXPECT_TRUE(automaton.accepts("a"));
        EXPECT_TRUE(automaton.accepts("b"));
        EXPECT_FALSE(automaton.accepts("c"));
        EXPECT_FALSE(automaton.accepts("aa"));
        EXPECT_FALSE(automaton.accepts("ab"));
        EXPECT_FALSE(automaton.accepts("ba"));
        EXPECT_FALSE(automaton.accepts("bb"));
        EXPECT_FALSE(automaton.accepts("aaa"));
    }
}

TEST(test_regex, test_regex_3) {
    Regex regex = *("a"_r + "b"_r);

    FiniteAutomaton automaton(regex);
    for(AutomatonConfigIterator config_iterator(automaton); config_iterator; config_iterator.next()) {
        EXPECT_TRUE(automaton.accepts("a"));
        EXPECT_TRUE(automaton.accepts("b"));
        EXPECT_FALSE(automaton.accepts("c"));
        EXPECT_TRUE(automaton.accepts("aa"));
        EXPECT_TRUE(automaton.accepts("ab"));
        EXPECT_TRUE(automaton.accepts("ba"));
        EXPECT_FALSE(automaton.accepts("bacb"));
        EXPECT_TRUE(automaton.accepts("bb"));
        EXPECT_TRUE(automaton.accepts("aaa"));
    }
}

TEST(test_regex, test_regex_4) {
    Regex regex = *("a"_r + "b"_r) * "c"_r;

    FiniteAutomaton automaton(regex);

    for(AutomatonConfigIterator config_iterator(automaton); config_iterator; config_iterator.next()) {
        EXPECT_FALSE(automaton.accepts("a"));
        EXPECT_FALSE(automaton.accepts("b"));
        EXPECT_TRUE(automaton.accepts("c"));
        EXPECT_FALSE(automaton.accepts("ab"));
        EXPECT_FALSE(automaton.accepts("aaa"));
        EXPECT_TRUE(automaton.accepts("abbac"));
        EXPECT_FALSE(automaton.accepts("cc"));
        EXPECT_FALSE(automaton.accepts("acaca"));
    }
}

TEST(test_regex, test_regex_5) {
    // The simplified automaton of this regex contains an epsilon loop,
    // which is a special case.
    Regex regex = *("a"_r + *"ab"_r);

    FiniteAutomaton automaton(regex);

    for(AutomatonConfigIterator config_iterator(automaton); config_iterator; config_iterator.next()) {
        EXPECT_FALSE(automaton.accepts("aaaabb"));
        EXPECT_FALSE(automaton.accepts("ba"));
        EXPECT_TRUE(automaton.accepts("aaaaab"));
        EXPECT_TRUE(automaton.accepts("abaa"));
    }
}


std::string regex_to_string(const Regex& regex) {
    std::stringstream ss;
    ss << regex;
    return ss.str();
}

TEST(test_regex, test_regex_printing) {
    EXPECT_EQ(regex_to_string(*("a"_r + "b"_r + Regex::empty()) * "c"_r), "((a + b + ε))*c") << "Regex printing failed";
    EXPECT_EQ(regex_to_string(Regex::zero()), "()") << "Regex printing failed";
}

TEST(test_regex, test_regex_graphviz_export) {
    FiniteAutomaton automaton;

    automaton.add_state(false);
    automaton.add_state(true);
    automaton.add_transition(0, 1, "a"_r);

    std::stringstream stream;
    stream << AutomatonGraphvizPrinter(automaton);
    std::string string = stream.str();

    EXPECT_TRUE(string.find("digraph") != std::string_view::npos);
    EXPECT_TRUE(string.find("0 -> 1") != std::string_view::npos);
}

TEST(test_regex, test_empty_zero_regex) {
    Regex zero_regex = (Regex::zero() * "a"_r) + "b"_r;
    Regex empty_regex = (Regex::empty() * "a"_r) + "b"_r;

    FiniteAutomaton zero_automaton(zero_regex);

    for(AutomatonConfigIterator config_iterator(zero_automaton); config_iterator; config_iterator.next()) {
        EXPECT_FALSE(zero_automaton.accepts("a"));
        EXPECT_TRUE(zero_automaton.accepts("b"));
        EXPECT_FALSE(zero_automaton.accepts(""));
    }

    FiniteAutomaton empty_automaton(zero_regex);

    for(AutomatonConfigIterator config_iterator(zero_automaton); config_iterator; config_iterator.next()) {
        EXPECT_TRUE(zero_automaton.accepts("b"));
        EXPECT_FALSE(zero_automaton.accepts("a"));
        EXPECT_FALSE(zero_automaton.accepts(""));
    }
}

TEST(test_automaton_states, test_automaton_states) {
    FiniteAutomaton automaton(*("aab"_r + "aac"_r));

    EXPECT_FALSE(automaton.is_simple()) << "Automaton should not be simple";
    EXPECT_FALSE(automaton.is_deterministic()) << "Automaton should not be deterministic";
    EXPECT_FALSE(automaton.is_complete()) << "Automaton should not be complete";

    AutomatonSimplifier(automaton).simplify();

    EXPECT_TRUE(automaton.is_simple()) << "Automaton should be simple";
    EXPECT_FALSE(automaton.is_complete()) << "Automaton should not be complete";
    EXPECT_FALSE(automaton.is_deterministic()) << "Automaton should not be deterministic";

    AutomatonCompleter(automaton).complete();

    EXPECT_TRUE(automaton.is_simple()) << "Automaton should be simple";
    EXPECT_TRUE(automaton.is_complete()) << "Automaton should be complete";
    EXPECT_FALSE(automaton.is_deterministic()) << "Automaton should not be deterministic";

    automaton = AutomatonDeterminator(automaton).determine();

    EXPECT_TRUE(automaton.is_simple()) << "Automaton should be simple";
    EXPECT_TRUE(automaton.is_complete()) << "Automaton should be complete";
    EXPECT_TRUE(automaton.is_deterministic()) << "Automaton should be deterministic";
}