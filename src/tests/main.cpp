
#include "gtest/gtest.h"
#include "../engine/regex.hpp"
#include "../engine/finite-automaton.hpp"
#include "../engine/automaton-simplifier.hpp"
#include "automaton-config-iterator.hpp"
#include "../engine/automaton-graphviz-printer.hpp"
#include "../engine/automaton-to-regex-converter.hpp"
#include "../engine/automaton-minifier.hpp"
#include "../engine/automaton-inverter.hpp"
#include "../engine/automaton-collapser.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

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

TEST(test_regex, test_regex_6) {
    // This regex has been found to cause a bug in the epsilon loop detection code.
    Regex regex = (*"ab"_r) * (*"b"_r) + *(("a"_r + "b"_r) * ("a"_r + "b"_r));

    FiniteAutomaton automaton(regex);

    for(AutomatonConfigIterator config_iterator(automaton); config_iterator; config_iterator.next()) {
        EXPECT_TRUE(automaton.accepts("abb"));
        EXPECT_TRUE(automaton.accepts("abab"));
        EXPECT_FALSE(automaton.accepts("aab"));
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

TEST(test_regex, test_regex_builder) {
    FiniteAutomaton automaton;

    size_t state_a = automaton.add_state(false);
    size_t state_b = automaton.add_state(true);

    automaton.add_transition(state_a, state_b, Regex(CharRegex('a')));
    automaton.add_transition(state_b, state_a, Regex(CharRegex('b')));
    automaton.add_transition(state_a, state_a, Regex(CharRegex('c')));
    automaton.add_transition(state_b, state_b, Regex(CharRegex('d')));

    Regex resulting_regex = AutomatonToRegexConverter(automaton).convert();

    FiniteAutomaton resulting_automaton(resulting_regex);

    AutomatonSimplifier(resulting_automaton).simplify();

    EXPECT_TRUE(resulting_automaton.accepts("a"));
    EXPECT_TRUE(resulting_automaton.accepts("ca"));
    EXPECT_TRUE(resulting_automaton.accepts("cca"));
    EXPECT_TRUE(resulting_automaton.accepts("cadd"));
    EXPECT_TRUE(resulting_automaton.accepts("ccaddbccaddbcca"));
    EXPECT_FALSE(resulting_automaton.accepts(""));
    EXPECT_FALSE(resulting_automaton.accepts("cc"));
}

TEST(test_automaton_states, test_automaton_states) {
    FiniteAutomaton automaton(*("aab"_r + "aac"_r));

    EXPECT_FALSE(automaton.is_simple()) << "Automaton should not be simple";
    EXPECT_FALSE(automaton.is_deterministic()) << "Automaton should not be deterministic";
    EXPECT_FALSE(automaton.is_complete()) << "Automaton should not be complete";

    AutomatonSimplifier(automaton).simplify();
    EpsilonRemover(automaton).simplify();

    EXPECT_TRUE(automaton.is_simple()) << "Automaton should be simple";
    EXPECT_FALSE(automaton.is_complete()) << "Automaton should not be complete";
    EXPECT_FALSE(automaton.is_deterministic()) << "Automaton should not be deterministic";
    EXPECT_FALSE(automaton.has_epsilon_transitions()) << "Automaton should not have any epsilon transitions";

    AutomatonCompleter(automaton).complete();

    EXPECT_TRUE(automaton.is_simple()) << "Automaton should be simple";
    EXPECT_TRUE(automaton.is_complete()) << "Automaton should be complete";
    EXPECT_FALSE(automaton.is_deterministic()) << "Automaton should not be deterministic";

    automaton = AutomatonDeterminator(automaton).determine();

    EXPECT_TRUE(automaton.is_simple()) << "Automaton should be simple";
    EXPECT_TRUE(automaton.is_complete()) << "Automaton should be complete";
    EXPECT_TRUE(automaton.is_deterministic()) << "Automaton should be deterministic";
}

TEST(test_automaton_states, test_automaton_eps_remove_1) {
    FiniteAutomaton automaton;

    size_t state_a = automaton.add_state(false);
    size_t state_b = automaton.add_state(false);
    size_t state_c = automaton.add_state(true);

    automaton.add_transition(state_a, state_b, Regex(CharRegex('a')));
    automaton.add_transition(state_b, state_c, Regex::empty());
    automaton.add_transition(state_c, state_a, Regex::empty());

    EXPECT_TRUE(automaton.is_simple());

    EpsilonRemover(automaton).simplify();

    EXPECT_TRUE(!automaton.has_epsilon_transitions());
    EXPECT_FALSE(automaton.accepts(""));
    EXPECT_FALSE(automaton.accepts("b"));
    EXPECT_TRUE(automaton.accepts("a"));
    EXPECT_TRUE(automaton.accepts("aaa"));
}

TEST(test_automaton_minify, test_automaton_minify_1) {
    FiniteAutomaton automaton(*"a"_r);

    automaton.extend_alphabet({'a', 'b'});

    AutomatonSimplifier(automaton).simplify();
    EpsilonRemover(automaton).simplify();
    AutomatonCompleter(automaton).complete();
    automaton = AutomatonDeterminator(automaton).determine();
    automaton = AutomatonMinifier(automaton).minify();

    EXPECT_TRUE(automaton.is_simple());
    EXPECT_TRUE(automaton.is_deterministic());
    EXPECT_TRUE(automaton.is_complete());

    EXPECT_EQ(automaton.get_states().size(), 2);
    EXPECT_TRUE(automaton.accepts("a"));
    EXPECT_FALSE(automaton.accepts("b"));
    EXPECT_TRUE(automaton.accepts(""));
}

TEST(test_automaton_minify, test_automaton_minify_2) {
    FiniteAutomaton automaton;

    automaton.extend_alphabet({'a', 'b'});

    size_t state_a = automaton.add_state(false);
    size_t state_b = automaton.add_state(true);
    size_t state_c = automaton.add_state(true);
    size_t state_d = automaton.add_state(true);
    size_t state_e = automaton.add_state(true);

    automaton.add_transition(state_a, state_b, Regex(CharRegex('a')));
    automaton.add_transition(state_b, state_c, Regex(CharRegex('a')));
    automaton.add_transition(state_c, state_d, Regex(CharRegex('a')));
    automaton.add_transition(state_d, state_e, Regex(CharRegex('a')));
    automaton.add_transition(state_e, state_e, Regex(CharRegex('a')));

    AutomatonCompleter(automaton).complete();

    automaton = AutomatonMinifier(automaton).minify();

    EXPECT_TRUE(automaton.is_simple());
    EXPECT_TRUE(automaton.is_deterministic());
    EXPECT_TRUE(automaton.is_complete());

    EXPECT_EQ(automaton.get_states().size(), 3);
    EXPECT_TRUE(automaton.accepts("a"));
    EXPECT_FALSE(automaton.accepts("b"));
    EXPECT_FALSE(automaton.accepts(""));
}

Regex to_regex(FiniteAutomaton& automaton) {
    AutomatonSimplifier(automaton).simplify();
    EpsilonRemover(automaton).simplify();
    AutomatonCompleter(automaton).complete();
    automaton = AutomatonDeterminator(automaton).determine();
    automaton = AutomatonMinifier(automaton).minify();
    AutomatonCollapser(automaton).collapse();
    return AutomatonToRegexConverter(automaton).convert();
}

TEST(test_automaton_minify, test_automaton_minify_3) {
    FiniteAutomaton automaton_1(*"a"_r);
    FiniteAutomaton automaton_2(*"a"_r + "aaa"_r + "a"_r);
    FiniteAutomaton automaton_3("a"_r * (*"a"_r + "aaa"_r + "a"_r) + "a"_r + Regex::empty());

    automaton_1.extend_alphabet({'a', 'b'});
    automaton_2.extend_alphabet({'a', 'b'});
    automaton_3.extend_alphabet({'a', 'b'});

    Regex regex_1 = to_regex(automaton_1);
    Regex regex_2 = to_regex(automaton_2);
    Regex regex_3 = to_regex(automaton_3);

    EXPECT_EQ(regex_1, regex_2);
    EXPECT_EQ(regex_2, regex_3);
}