#include <iostream>
#include "../engine/regex.hpp"
#include "../engine/finite-automaton.hpp"
#include "../engine/automaton-simplifier.hpp"
#include "../engine/epsilon-remover.hpp"
#include "../engine/automaton-optimizer.hpp"
#include "../engine/automaton-determinator.hpp"
#include "../engine/automaton-inverter.hpp"
#include "../engine/automaton-collapser.hpp"
#include "../engine/automaton-to-regex-converter.hpp"
#include "../engine/automaton-minifier.hpp"
#include "../engine/automaton-graphviz-printer.hpp"

Regex invert_regex(const Regex& regex, const std::set<char>& alphabet = {}) {
    FiniteAutomaton automaton(regex);
    automaton.extend_alphabet(alphabet);
    AutomatonSimplifier(automaton).simplify();

    EpsilonRemover(automaton).simplify();
    AutomatonOptimizer(automaton).optimize();
    AutomatonCompleter(automaton).complete();
    automaton = AutomatonDeterminator(automaton).determine();

    automaton = AutomatonMinifier(automaton).minify();

    std::cout << AutomatonGraphvizPrinter(automaton) << "\n";

    AutomatonInverter(automaton).invert();

    AutomatonCollapser(automaton).collapse();

    return AutomatonToRegexConverter(automaton).convert();
}

int main() {

//    Regex regex = *("ab"_r + "ba"_r) * (Regex() + "a"_r + "ba"_r);
//    Regex regex = *"ab"_r * *"b"_r + *(("a"_r + "b"_r) * ("a"_r + "b"_r));
//    Regex regex = "a"_r * *((*"ba"_r * *"a"_r * *"ab"_r) + "a"_r);

//    Regex regex = "a"_r * *(*("ba"_r) * "a"_r * *("ab"_r) + "a"_r);

// a ((ba)∗a(ab)∗ + a)∗

// (a(ab + ba)∗b(a + ba)∗)

//    Regex regex = ("a"_r * *("ab"_r + "ba"_r) * "b"_r * *("a"_r + "ba"_r)) * *("a"_r * *("ab"_r + "ba"_r) * "b"_r * *("a"_r + "ba"_r));
//
//    std::cout << regex << "\n";
//
//    std::cout << invert_regex(regex, {'a', 'b'}) << "\n";

    //FiniteAutomaton automaton("a"_r * *"a"_r * *("a"_r + "ba"_r) * *"b"_r * ("b"_r+"a"_r));
//    FiniteAutomaton automaton(*"b"_r * *"a"_r * "bb"_r * (*"a"_r + "aa"_r));

    Regex reg = "rax"_r + "rbx"_r + "rcx"_r + "rdx"_r + "rsi"_r + "rdi"_r + "rbp"_r + "rsp"_r + "r8"_r + "r9"_r + "r10"_r + "r11"_r + "r12"_r + "r13"_r + "r14"_r + "r15"_r;
    Regex digit = "0"_r + "1"_r + "2"_r + "3"_r + "4"_r + "5"_r + "6"_r + "7"_r + "8"_r + "9"_r;
    Regex number = digit + *digit;
    Regex whitespace = " "_r + "\n"_r;

    Regex no_register_instructions = "nop"_r;
    Regex one_register_instructions = "push"_r + "pop"_r + "inc"_r + "dec"_r + "neg"_r + "not"_r + "mul"_r + "div"_r + "call"_r + "jmp"_r + "jz"_r + "jnz"_r + "je"_r + "jne"_r + "jg"_r + "jge"_r + "jl"_r + "jle"_r + "ja"_r + "jae"_r + "jb"_r + "jbe"_r + "ret"_r + "leave"_r;
    Regex two_registers_instructions = "mov"_r + "add"_r + "sub"_r + "and"_r + "or"_r + "xor"_r + "cmp"_r + "test"_r + "imul"_r + "idiv"_r;
    Regex register_number_instructions = "mov"_r + "add"_r + "sub"_r + "and"_r + "or"_r + "xor"_r + "cmp"_r + "test"_r + "imul"_r + "idiv"_r;

    Regex program =
        *(
            *whitespace
            + (
                no_register_instructions
                + *whitespace
                + *number
            )
            + *whitespace
            + *(
                +whitespace
                + (
                    one_register_instructions
                    + *whitespace
                    + reg
                )
                + *whitespace
                + *(
                    +whitespace
                    + (
                        two_registers_instructions
                        + *whitespace
                        + reg
                        + *whitespace
                        + reg
                    )
                    + *whitespace
                    + *(
                        +whitespace
                        + (
                            register_number_instructions
                            + *whitespace
                            + reg
                            + *whitespace
                            + number
                        )
                        + *whitespace
                    )
                )
            )
        );

    std::cout << program << "\n";

//    FiniteAutomaton automaton(program);

//    int state_start = automaton.add_state(true);
//    int state_a_a = automaton.add_state(true);
//    int state_a_b = automaton.add_state(false);
//    int state_b_a = automaton.add_state(false);
//    int state_b_b = automaton.add_state(true);
//
//    automaton.add_transition(state_a_a, state_a_a, Regex(CharRegex('a')));
//    automaton.add_transition(state_b_a, state_b_a, Regex(CharRegex('a')));
//    automaton.add_transition(state_a_b, state_a_b, Regex(CharRegex('b')));
//    automaton.add_transition(state_b_b, state_b_b, Regex(CharRegex('b')));
//
//    automaton.add_transition(state_a_a, state_a_b, Regex(CharRegex('b')));
//    automaton.add_transition(state_b_a, state_b_b, Regex(CharRegex('b')));
//    automaton.add_transition(state_a_b, state_a_a, Regex(CharRegex('a')));
//    automaton.add_transition(state_b_b, state_b_a, Regex(CharRegex('a')));
//
//    automaton.add_transition(state_start, state_a_a, Regex(CharRegex('a')));
//    automaton.add_transition(state_start, state_b_a, Regex(CharRegex('b')));

//    int a0 = automaton.add_state(true);
//    int a1 = automaton.add_state(false);
//    int a2 = automaton.add_state(false);
//    int a3 = automaton.add_state(true);
//    int a4 = automaton.add_state(true);
//
//    automaton.add_transition(a0, a1, "a"_r);
//    automaton.add_transition(a1, a1, "a"_r + "b"_r);
//    automaton.add_transition(a1, a3, "a"_r);
//    automaton.add_transition(a0, a2, "b"_r);
//    automaton.add_transition(a2, a2, "b"_r + "a"_r);
//    automaton.add_transition(a2, a4, "b"_r);

//    FiniteAutomaton automaton("a"_r * *(*("ba"_r) * "a"_r * *("ab"_r) + "a"_r));

//    AutomatonSimplifier(automaton).simplify();
//
//    EpsilonRemover(automaton).simplify();
//    AutomatonOptimizer(automaton).optimize();
//    AutomatonCompleter(automaton).complete();
//    automaton = AutomatonDeterminator(automaton).determine();
//    AutomatonInverter(automaton).invert();
//    automaton = AutomatonMinifier(automaton).minify();
//
//    std::cout << AutomatonGraphvizPrinter(automaton) << "\n";
//
//    std::cout << AutomatonGraphvizPrinter(automaton) << "\n";
//
//    AutomatonCollapser(automaton).collapse();

//    assert(!automaton.accepts("aa"));
//    assert(!automaton.accepts("baababba"));
//    assert(!automaton.accepts("baaababbab"));
//    assert(automaton.accepts("baababbaa"));
//    assert(automaton.accepts("aaaa"));
//    assert(automaton.accepts("aaaabaaaa"));
//    assert(automaton.accepts("aaaababaaaa"));

//    AutomatonCollapser(automaton).collapse();

//    std::cout << AutomatonToRegexConverter(automaton).convert() << "\n";

    return 0;
}
