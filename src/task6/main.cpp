
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

Regex parse_regex_invert_polish_notation(const std::string& regex) {
    std::stack<Regex> stack;
    for (char i : regex) {
        if (i == 'a' || i == 'b' || i == 'c') {
            stack.push(Regex(CharRegex(i)));
        } else if (i == '*') {
            Regex r = stack.top();
            stack.pop();
            stack.push(*r);
        } else if (i == '+') {
            Regex r1 = stack.top();
            stack.pop();
            Regex r2 = stack.top();
            stack.pop();
            stack.push(r1 + r2);
        } else if (i == '1') {
            stack.push(Regex());
        } else if (i == '.') {
            Regex r1 = stack.top();
            stack.pop();
            Regex r2 = stack.top();
            stack.pop();
            stack.push(r1 * r2);
        }
    }
    return stack.top();
}

bool is_final_state_reachable(FiniteAutomaton& automaton, size_t start_index) {
    if(automaton.get_states()[start_index].is_final) {
        return true;
    }

    std::set<size_t> current_states;
    current_states.insert(start_index);

    while(true) {
        std::set<size_t> next_states;
        for(auto state_index : current_states) {
            auto& state = automaton.get_states()[state_index];
            for(auto& transition : state.transitions) {
                if(automaton.get_states()[transition.target_index].is_final) {
                    return true;
                }
                next_states.insert(transition.target_index);
            }
        }
        size_t old_states_size = current_states.size();
        current_states.insert(next_states.begin(), next_states.end());
        if(current_states.size() == old_states_size) {
            break;
        }
    };

    return false;
}

int main(int argc, const char** argv) {
    if(argc < 4) {
        std::cout << "Usage: " << argv[0] << " <regex> <char> <k>" << std::endl;
        return 1;
    }

    std::string input_string = argv[1];
    char ch = argv[2][0];
    int k = std::stoi(argv[3]);

    Regex regex = parse_regex_invert_polish_notation(input_string);

    FiniteAutomaton automaton(regex);
    automaton.extend_alphabet({'a', 'b', 'c'});
    AutomatonSimplifier(automaton).simplify();
    EpsilonRemover(automaton).simplify();
    AutomatonCompleter(automaton).complete();
    automaton = AutomatonDeterminator(automaton).determine();

    size_t current_state = automaton.get_start_state_index();

    for(int i = 0; i < k; i++) {
        int transition = automaton.find_transition(ch, current_state);
        current_state = automaton.get_transition(current_state, transition).target_index;
    }

    // Determine whether the final state is reachable from the current state

    if(is_final_state_reachable(automaton, current_state)) {
        std::cout << "YES\n";
    } else {
        std::cout << "NO\n";
    }
}
