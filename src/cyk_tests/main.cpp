
#include <sstream>
#include <gtest/gtest.h>
#include "../cyk/engine/grammar.hpp"
#include "../cyk/engine/grammar-parser.hpp"
#include "../cyk/engine/grammar-printer.hpp"
#include "../cyk/engine/processors/useless-symbol-remover.hpp"
#include "../cyk/engine/processors/mixed-rule-remover.hpp"
#include "../cyk/engine/processors/long-rule-remover.hpp"
#include "../cyk/engine/processors/epsilon-creating-remover.hpp"
#include "../cyk/engine/processors/chain-rule-remover.hpp"
#include "../cyk/engine/cyk-algorithm.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

std::string sort_alphabet(std::string alphabet) {
    std::sort(alphabet.begin(), alphabet.end());
    return alphabet;
}

std::string sort_alphabet(const std::unordered_set<char> &a) {
    std::string a_string(a.begin(), a.end());
    return sort_alphabet(a_string);
}

void check_rules(Grammar &grammar, const std::vector<GrammarRule> &expected_rules, const std::string& start) {

    for (int i = 0; i < expected_rules.size(); i++) {
        bool found = false;
        for (int j = 0; j < grammar.get_rules().size(); j++) {
            if (grammar.get_rules()[j] == expected_rules[i]) {
                found = true;
                break;
            }
        }

        EXPECT_TRUE(found) << "Rule " << i << " not found in expected rules. The grammar is:\n"
                           << GrammarPrinter(grammar);
    }

    for (int i = 0; i < grammar.get_rules().size(); i++) {
        bool found = false;
        for (int j = 0; j < expected_rules.size(); j++) {
            if (grammar.get_rules()[i] == expected_rules[j]) {
                found = true;
                break;
            }
        }

        EXPECT_TRUE(found) << "There is an excessive rule in the grammar. The grammar is:\n"
                           << GrammarPrinter(grammar);
    }

    // Make sure that the start nonterminal is the first one

    EXPECT_EQ(start, grammar.get_start_nonterminal().id)
            << "The start nonterminal is not the first one. The grammar is:\n" << GrammarPrinter(grammar);
}

TEST(test_parse, test_parse_1) {
    std::stringstream input;

    input << "<S> ::= a<S>b" << std::endl;
    input << "<S> ::= a" << std::endl;
    input << "<S> ::= b" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    check_rules(grammar, {
            GrammarRule({NonTerminal("S")}, {Terminal('a'), NonTerminal("S"), Terminal('b')}),
            GrammarRule({NonTerminal("S")}, {Terminal('a')}),
            GrammarRule({NonTerminal("S")}, {Terminal('b')})
    }, "S");

    EXPECT_EQ(sort_alphabet(grammar.get_alphabet()), "ab");
}

TEST(test_parse, test_parse_2) {
    // Test backslash

    std::stringstream input;

    input << "<S> ::= <whitespace><special_char>" << std::endl;
    input << "<whitespace> ::= \\n" << std::endl;
    input << "<whitespace> ::= \\w" << std::endl;
    input << std::endl; // Empty line
    input << "<special_char> ::= <\\<>" << std::endl;
    input << R"(<\<> ::= \<)" << std::endl;
    input << R"(<\<> ::= \>)" << std::endl;
    input << R"(<\<> ::= \\)" << std::endl;
    input << R"(<\<> ::= \t)" << std::endl;
    input << R"(<\<> ::= \r)" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    check_rules(grammar, {
            GrammarRule({NonTerminal("S")}, { NonTerminal("whitespace"), NonTerminal("special_char") }),
            GrammarRule({NonTerminal("whitespace")}, {Terminal('\n')}),
            GrammarRule({NonTerminal("whitespace")}, {Terminal(' ')}),
            GrammarRule({NonTerminal("special_char")}, {NonTerminal("<")}),
            GrammarRule({NonTerminal("<")}, {Terminal('<')}),
            GrammarRule({NonTerminal("<")}, {Terminal('>')}),
            GrammarRule({NonTerminal("<")}, {Terminal('\\')}),
            GrammarRule({NonTerminal("<")}, {Terminal('\t')}),
            GrammarRule({NonTerminal("<")}, {Terminal('\r')})
    }, "S");
}



TEST(test_parse, test_parse_3) {
    // Test 'or' operator

    std::stringstream input;

    input << "<S> ::= a | b" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    check_rules(grammar, {
            GrammarRule({NonTerminal("S")}, {Terminal('a')}),
            GrammarRule({NonTerminal("S")}, {Terminal('b')}),
    }, "S");
}

TEST(test_print, test_print_1) {
    Grammar grammar;
    grammar.add_rule(GrammarRule({NonTerminal("S")}, {Terminal('a'), NonTerminal("S"), Terminal('b')}));
    grammar.add_rule(GrammarRule({NonTerminal("S")}, {Terminal('a')}));
    grammar.add_rule(GrammarRule({NonTerminal("S")}, {Terminal('b')}));
    grammar.add_rule(GrammarRule({NonTerminal("S")}, {Terminal('\n')}));
    grammar.add_rule(GrammarRule({NonTerminal("S")}, {Terminal(' ')}));
    grammar.add_rule(GrammarRule({NonTerminal("S")}, {Terminal('<')}));
    grammar.add_rule(GrammarRule({NonTerminal("S")}, {Terminal('>')}));
    grammar.add_rule(GrammarRule({NonTerminal("S")}, {Terminal('|')}));
    grammar.add_rule(GrammarRule({NonTerminal("S")}, {Terminal('\\')}));
    grammar.add_rule(GrammarRule({NonTerminal("S")}, {Terminal('\t')}));


    std::stringstream output;

    output << GrammarPrinter(grammar);

    ASSERT_EQ(output.str(), "<S> ::= a<S>b\n<S> ::= a\n<S> ::= b\n<S> ::= \\n\n<S> ::= \\w\n<S> ::= \\<\n<S> ::= \\>\n<S> ::= \\|\n<S> ::= \\\\\n<S> ::= \\t\n");
}

TEST(test_processor, test_useless_symbol_remove_1) {
    std::stringstream input;

    input << "<S> ::= a<S>b" << std::endl;
    input << "<S> ::= a" << std::endl;
    input << "<S> ::= b" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    // There are no useless symbols in this grammar, so
    // expect the grammar to be unchanged.
    Grammar new_grammar = UselessSymbolRemover(grammar).remove();

    check_rules(grammar, new_grammar.get_rules(), "S");
}

TEST(test_processor, test_useless_symbol_remove_2) {
    std::stringstream input;

    input << "<S> ::= a<S>b" << std::endl;
    input << "<S> ::= <M>" << std::endl;
    input << "<M> ::= a<M>" << std::endl;
    input << "<E> ::= a" << std::endl;
    input << "<S> ::= a" << std::endl;
    input << "<S> ::= b" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    // E is unreachable, although it's creative
    // M is not creative, but it's reachable
    // Expect them both to be removed.

    grammar = UselessSymbolRemover(grammar).remove();

    check_rules(grammar, {
            GrammarRule({NonTerminal("S")}, {Terminal('a'), NonTerminal("S"), Terminal('b')}),
            GrammarRule({NonTerminal("S")}, {Terminal('a')}),
            GrammarRule({NonTerminal("S")}, {Terminal('b')}),
    }, "S");
    EXPECT_EQ(sort_alphabet(grammar.get_alphabet()), "ab");
}

TEST(test_processor, test_useless_symbol_remove_3) {
    std::stringstream input;

    input << "<S> ::= a" << std::endl;
    input << "<T> ::= a" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    // S is unreachable
    // Expect it to be removed.

    grammar = UselessSymbolRemover(grammar).remove();

    check_rules(grammar, {
            GrammarRule({NonTerminal("S")}, {Terminal('a')}),
    }, "S");
    EXPECT_EQ(sort_alphabet(grammar.get_alphabet()), "a");
}

TEST(test_processor, test_mixed_rule_remove_1) {
    std::stringstream input;

    input << "<S> ::= a<S>b" << std::endl;
    input << "<S> ::= a" << std::endl;
    input << "<S> ::= b" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    // There is a mixed rule: S ::= aSb
    // It should be transformed into a new rule set:
    // <S> ::= <A><S><B>
    // <A> ::= a
    // <B> ::= b

    MixedRuleRemover(grammar).remove();

    check_rules(grammar, {
            GrammarRule({NonTerminal("S")}, {NonTerminal("A"), NonTerminal("S"), NonTerminal("B")}),
            GrammarRule({NonTerminal("A")}, {Terminal('a')}),
            GrammarRule({NonTerminal("B")}, {Terminal('b')}),
            GrammarRule({NonTerminal("S")}, {Terminal('a')}),
            GrammarRule({NonTerminal("S")}, {Terminal('b')})
    }, "S");
    EXPECT_EQ(sort_alphabet(grammar.get_alphabet()), "ab");
}

TEST(test_processor, test_long_rule_remove_1) {
    std::stringstream input;

    input << "<S> ::= <A><B><C><D>" << std::endl;
    input << "<A> ::= a" << std::endl;
    input << "<B> ::= b" << std::endl;
    input << "<C> ::= c" << std::endl;
    input << "<D> ::= d" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    // There is a long rule: S ::= ABCD
    // It should be transformed into a new rule set:
    // S ::= AE
    // E ::= BF
    // F ::= CD

    LongRuleRemover(grammar).remove();

    check_rules(grammar, {
            {{NonTerminal("S")}, {NonTerminal("A"), NonTerminal("tmp")}},
            {{NonTerminal("tmp")}, {NonTerminal("B"), NonTerminal("tmp_1")}},
            {{NonTerminal("tmp_1")}, {NonTerminal("C"), NonTerminal("D")}},
            {{NonTerminal("A")}, {Terminal('a')}},
            {{NonTerminal("B")}, {Terminal('b')}},
            {{NonTerminal("C")}, {Terminal('c')}},
            {{NonTerminal("D")}, {Terminal('d')}},
    }, "S");
}

TEST(test_processor, test_epsilon_remover_1) {
    std::stringstream input;

    input << "<S> ::= a<S>b" << std::endl;
    input << "<S> ::= a" << std::endl;
    input << "<S> ::= b" << std::endl;

    Grammar grammar = GrammarParser().parse(input);
    Grammar saved_gr = grammar;

    // There is no epsilon rule in this grammar, so
    // expect the grammar to be unchanged.
    EpsilonCreatingRemover(grammar).remove();

    check_rules(grammar, saved_gr.get_rules(), "S");
}

TEST(test_processor, test_epsilon_remover_2) {
    std::stringstream input;

    input << "<S> ::= <D><F>" << std::endl;
    input << "<D> ::= " << std::endl;
    input << "<F> ::= a" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    // There is an epsilon rule in this grammar, so
    // expect the grammar to be changed.
    EpsilonCreatingRemover(grammar).remove();

    check_rules(grammar, {
            GrammarRule({NonTerminal("S")}, {NonTerminal("D"), NonTerminal("F")}),
            GrammarRule({NonTerminal("F")}, {Terminal('a')}),
            GrammarRule({NonTerminal("S")}, {NonTerminal("F")}),
    }, "S");
    EXPECT_EQ(sort_alphabet(grammar.get_alphabet()), "a");
}

TEST(test_processor, test_epsilon_remover_3) {
    std::stringstream input;

    input << "<S> ::= <D><F>" << std::endl;
    input << "<S> ::= " << std::endl;
    input << "<D> ::= " << std::endl;
    input << "<F> ::= a" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    // There is an epsilon rule in this grammar, so
    // expect the grammar to be changed.
    EpsilonCreatingRemover(grammar).remove();

    check_rules(grammar, {
            GrammarRule({NonTerminal("S_1")}, {NonTerminal("S")}),
            GrammarRule({NonTerminal("S_1")}, {}),
            GrammarRule({NonTerminal("S")}, {NonTerminal("D"), NonTerminal("F")}),
            GrammarRule({NonTerminal("S")}, {NonTerminal("F")}),
            GrammarRule({NonTerminal("F")}, {Terminal('a')}),
    }, "S_1");
    EXPECT_EQ(sort_alphabet(grammar.get_alphabet()), "a");
}

TEST(test_processor, test_chain_rule_remover_1) {
    std::stringstream input;

    input << "<S> ::= <T>" << std::endl;
    input << "<T> ::= a" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    ChainRuleRemover(grammar).remove();

    check_rules(grammar, {
            GrammarRule({NonTerminal("S")}, {Terminal('a')}),
    }, "S");
}

TEST(test_cyk, test_cyk_1) {
    std::stringstream input;

    input << "<S> ::= <A><B>" << std::endl;
    input << "<A> ::= a" << std::endl;
    input << "<B> ::= b" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    CYKAlgorithm cyk(grammar);

    EXPECT_TRUE(cyk.check("ab"));
    EXPECT_FALSE(cyk.check("ba"));
    EXPECT_FALSE(cyk.check("a"));
    EXPECT_FALSE(cyk.check("b"));
    EXPECT_FALSE(cyk.check("aa"));
    EXPECT_FALSE(cyk.check("bb"));
    EXPECT_FALSE(cyk.check("aab"));
    EXPECT_FALSE(cyk.check("aba"));
    EXPECT_FALSE(cyk.check("baa"));
    EXPECT_FALSE(cyk.check("abb"));
    EXPECT_FALSE(cyk.check("bba"));
    EXPECT_FALSE(cyk.check("bbb"));
}