
#include <sstream>

#include "../engine/regex.hpp"
#include "../engine/finite-automaton.hpp"
#include "../engine/automaton-simplifier.hpp"
#include "automaton-config-iterator.hpp"
#include "../engine/automaton-graphviz-printer.hpp"

void test_regex_1(Tester& tester) {
    Regex regex = *("a"_r);

    FiniteAutomaton automaton(regex);

    for(AutomatonConfigIterator config_iterator(automaton, tester); config_iterator; config_iterator.next()) {
        tester.assert_automaton_accepts(automaton, "");
        tester.assert_automaton_accepts(automaton, "a");
        tester.assert_automaton_rejects(automaton, "b");
        tester.assert_automaton_accepts(automaton, "aa");
        tester.assert_automaton_rejects(automaton, "ab");
        tester.assert_automaton_rejects(automaton, "ba");
        tester.assert_automaton_rejects(automaton, "bb");
        tester.assert_automaton_accepts(automaton, "aaa");
    }
}

void test_regex_2(Tester& tester) {
    Regex regex = "a"_r + "b"_r;

    FiniteAutomaton automaton(regex);

    for(AutomatonConfigIterator config_iterator(automaton, tester); config_iterator; config_iterator.next()) {
        tester.assert_automaton_rejects(automaton, "");
        tester.assert_automaton_accepts(automaton, "a");
        tester.assert_automaton_accepts(automaton, "b");
        tester.assert_automaton_rejects(automaton, "c");
        tester.assert_automaton_rejects(automaton, "aa");
        tester.assert_automaton_rejects(automaton, "ab");
        tester.assert_automaton_rejects(automaton, "ba");
        tester.assert_automaton_rejects(automaton, "bb");
        tester.assert_automaton_rejects(automaton, "aaa");
    }
}

void test_regex_3(Tester& tester) {
    Regex regex = *("a"_r + "b"_r);

    FiniteAutomaton automaton(regex);
    for(AutomatonConfigIterator config_iterator(automaton, tester); config_iterator; config_iterator.next()) {
        tester.assert_automaton_accepts(automaton, "a");
        tester.assert_automaton_accepts(automaton, "b");
        tester.assert_automaton_rejects(automaton, "c");
        tester.assert_automaton_accepts(automaton, "aa");
        tester.assert_automaton_accepts(automaton, "ab");
        tester.assert_automaton_accepts(automaton, "ba");
        tester.assert_automaton_rejects(automaton, "bacb");
        tester.assert_automaton_accepts(automaton, "bb");
        tester.assert_automaton_accepts(automaton, "aaa");
    }
}

void test_regex_4(Tester& tester) {
    Regex regex = *("a"_r + "b"_r) * "c"_r;

    FiniteAutomaton automaton(regex);

    for(AutomatonConfigIterator config_iterator(automaton, tester); config_iterator; config_iterator.next()) {
        tester.assert_automaton_rejects(automaton, "a");
        tester.assert_automaton_rejects(automaton, "b");
        tester.assert_automaton_accepts(automaton, "c");
        tester.assert_automaton_rejects(automaton, "ab");
        tester.assert_automaton_rejects(automaton, "aaa");
        tester.assert_automaton_accepts(automaton, "abbac");
        tester.assert_automaton_rejects(automaton, "cc");
        tester.assert_automaton_rejects(automaton, "acaca");
    }
}

void test_regex_5(Tester& tester) {
    // The simplified automaton of this regex contains an epsilon loop,
    // which is a special case.
    Regex regex = *("a"_r + *"ab"_r);

    FiniteAutomaton automaton(regex);

    for(AutomatonConfigIterator config_iterator(automaton, tester); config_iterator; config_iterator.next()) {
        tester.assert_automaton_rejects(automaton, "aaaabb");
        tester.assert_automaton_rejects(automaton, "ba");
        tester.assert_automaton_accepts(automaton, "aaaaab");
        tester.assert_automaton_accepts(automaton, "abaa");
    }
}


std::string regex_to_string(const Regex& regex) {
    std::stringstream ss;
    ss << regex;
    return ss.str();
}

void test_regex_printing(Tester& tester) {
    tester.assert_true(regex_to_string(*("a"_r + "b"_r + Regex::empty()) * "c"_r) == "((a+b+ε))*c", "Regex printing failed");
    tester.assert_true(regex_to_string(Regex::zero()) == "()", "Regex printing failed");
}

void test_graphviz_export(Tester& tester) {
    FiniteAutomaton automaton;

    automaton.add_state(false);
    automaton.add_state(true);
    automaton.add_transition(0, 1, "a"_r);

    std::stringstream stream;
    stream << AutomatonGraphvizPrinter(automaton);
    std::string string = stream.str();

    tester.assert_has_substring(string, "digraph");
    tester.assert_has_substring(string, "0 -> 1");
}

void test_empty_zero_regex(Tester& tester) {
    Regex zero_regex = (Regex::zero() * "a"_r) + "b"_r;
    Regex empty_regex = (Regex::empty() * "a"_r) + "b"_r;

    FiniteAutomaton zero_automaton(zero_regex);

    for(AutomatonConfigIterator config_iterator(zero_automaton, tester); config_iterator; config_iterator.next()) {
        tester.assert_automaton_rejects(zero_automaton, "a");
        tester.assert_automaton_accepts(zero_automaton, "b");
        tester.assert_automaton_rejects(zero_automaton, "");
    }

    FiniteAutomaton empty_automaton(zero_regex);

    for(AutomatonConfigIterator config_iterator(zero_automaton, tester); config_iterator; config_iterator.next()) {
        tester.assert_automaton_accepts(zero_automaton, "b");
        tester.assert_automaton_rejects(zero_automaton, "a");
        tester.assert_automaton_rejects(zero_automaton, "");
    }
}

void test_automaton_states(Tester& tester) {
    FiniteAutomaton automaton(*("aab"_r + "aac"_r));

    tester.assert_false(automaton.is_simple(), "Automaton should not be simple");
    tester.assert_false(automaton.is_deterministic(), "Automaton should not be deterministic");
    tester.assert_false(automaton.is_complete(), "Automaton should not be complete");

    AutomatonSimplifier(automaton).simplify();

    tester.assert_true(automaton.is_simple(), "Automaton should be simple");
    tester.assert_false(automaton.is_complete(), "Automaton should not be complete");
    tester.assert_false(automaton.is_deterministic(), "Automaton should not be deterministic");

    AutomatonCompleter(automaton).complete();

    tester.assert_true(automaton.is_simple(), "Automaton should be simple");
    tester.assert_true(automaton.is_complete(), "Automaton should be complete");
    tester.assert_false(automaton.is_deterministic(), "Automaton should not be deterministic");

    automaton = AutomatonDeterminator(automaton).determine();

    tester.assert_true(automaton.is_simple(), "Automaton should be simple");
    tester.assert_true(automaton.is_complete(), "Automaton should be complete");
    tester.assert_true(automaton.is_deterministic(), "Automaton should be deterministic");
}

int main() {
    Tester tester;

    test_automaton_states(tester);
    test_regex_1(tester);
    test_regex_2(tester);
    test_regex_3(tester);
    test_regex_4(tester);
    test_regex_5(tester);
    test_regex_printing(tester);
    test_empty_zero_regex(tester);
    test_graphviz_export(tester);
}