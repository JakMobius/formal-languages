
#include <iostream>
#include <stack>
#include "../engine/regex.hpp"
#include "../engine/finite-automaton.hpp"
#include "../engine/automaton-simplifier.hpp"
#include "../engine/epsilon-remover.hpp"
#include "../engine/automaton-completer.hpp"
#include "../engine/automaton-minifier.hpp"
#include "../engine/automaton-inverter.hpp"
#include "../engine/automaton-collapser.hpp"
#include "../engine/automaton-determinator.hpp"
#include "task6.hpp"

int main(int argc, const char** argv) {
    if(argc < 4) {
        std::cout << "Usage: " << argv[0] << " <regex> <char> <k>" << std::endl;
        return 1;
    }

    std::string input_string = argv[1];
    char ch = argv[2][0];
    int k = std::stoi(argv[3]);

    int steps = Task6::solve(input_string, ch, k);

    if(steps == -1) {
        std::cout << "NO\n";
        return 0;
    }

    std::cout << steps << "\n";
    return 0;
}
