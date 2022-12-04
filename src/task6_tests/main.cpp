
#include "gtest/gtest.h"
#include "../task6/task6.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(test_value, test_answer_1) {
    std::string input_string = "aa.";
    char ch = 'a';
    int k = 2;

    int steps = Task6::solve(input_string, ch, k);

    ASSERT_EQ(steps, 2);
}

TEST(test_value, test_answer_2) {
    std::string input_string = "aa.";
    char ch = 'b';
    int k = 2;

    int steps = Task6::solve(input_string, ch, k);

    ASSERT_EQ(steps, -1);
}

TEST(test_answer, test_answer_3) {
    std::string input_string = "acb..bab.c.*.ab.ba.+.+*a.";
    char ch = 'a';
    int k = 2;

    int steps = Task6::solve(input_string, ch, k);

    ASSERT_EQ(steps, 4);
}