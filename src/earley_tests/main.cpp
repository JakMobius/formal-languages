
#include <gtest/gtest.h>
#include "../cyk/engine/grammar.hpp"
#include "../cyk/engine/grammar-parser.hpp"
#include "../earley/engine/earley-parser.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(GeneralTest, EarleyTestCPS) {

    std::stringstream input;

    input << "<S> ::= <X>" << std::endl;
    input << "<X> ::= <X>a<X>b" << std::endl;
    input << "<X> ::= " << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    EarleyParser parser(grammar);

    EXPECT_TRUE(parser.parse(""));
    EXPECT_TRUE(parser.parse("aabb"));
    EXPECT_TRUE(parser.parse("abab"));
    EXPECT_TRUE(parser.parse("aababaabbb"));
    EXPECT_TRUE(parser.parse("aabbabababaaaaaababbaaaabbbabbbbbaabbbab"));
    EXPECT_TRUE(parser.parse("aaaaaaabbbbabaaabbbbabaabaababaaaaabababbbabbabbaaabaaaaaababbbbbabaababbbbbbbbb"));

    EXPECT_FALSE(parser.parse("bbaaabab"));
    EXPECT_FALSE(parser.parse("aabbabababaabbaaaaabbabaaaabbbabbbbbaabbbab"));
    EXPECT_FALSE(parser.parse("aaaaaaabbbbabaaababbabaabaababaaaaababaababbbabbabbaaabbaaaaababbbbbabaababbbbbbbbb"));
    EXPECT_FALSE(parser.parse("morgenshtern"));
}

TEST(GeneralTest, EarleyTestPalindroms) {

    std::stringstream input;

    input << "<S> ::= <X>\n" << std::endl;
    input << "<X> ::= a<X>a\n" << std::endl;
    input << "<X> ::= b<X>b\n" << std::endl;
    input << "<X> ::= c<X>c\n" << std::endl;
    input << "<X> ::= c<X>c\n" << std::endl;
    input << "<X> ::= o<X>o\n" << std::endl;
    input << "<X> ::= a\n" << std::endl;
    input << "<X> ::= b\n" << std::endl;
    input << "<X> ::= c\n" << std::endl;
    input << "<X> ::= o\n" << std::endl;
    input << "<X> ::= \n" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    EarleyParser parser(grammar);

    EXPECT_TRUE(parser.parse("aba"));
    EXPECT_TRUE(parser.parse("abacaba"));
    EXPECT_TRUE(parser.parse("abacabaoooabacababbbabacabaoooabacabacccabacabaoooabacababbbabacabaoooabacaba"));
    EXPECT_TRUE(parser.parse("aaabaaa"));

    EXPECT_FALSE(parser.parse("ba"));
    EXPECT_FALSE(parser.parse("baa"));
    EXPECT_FALSE(parser.parse("morgenshtern"));
    EXPECT_FALSE(parser.parse("abacabaobbabacabaoooabacabaobbabacaba"));
}

TEST(GeneralTest, NoEpsGrammar) {

    std::stringstream input;

    input << "<S> ::= <X>" << std::endl;
    input << "<X> ::= <Y>" << std::endl;
    input << "<Y> ::= <Z>" << std::endl;
    input << "<Z> ::= slavamerlou" << std::endl;
    input << "<Z> ::= morgenshtern" << std::endl;

    Grammar grammar = GrammarParser().parse(input);

    EarleyParser parser(grammar);

    EXPECT_TRUE(parser.parse("slavamerlou"));
    EXPECT_TRUE(parser.parse("morgenshtern"));

    EXPECT_FALSE(parser.parse(""));
    EXPECT_FALSE(parser.parse("kek"));
}