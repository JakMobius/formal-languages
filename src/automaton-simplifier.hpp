
#include "finite-automaton.hpp"

class AutomatonSimplifier {
public:
    AutomatonSimplifier(FiniteAutomaton &automaton) : automaton(automaton) {

    }

    bool get_long_transition(int &state, int &transition_index) const;

    void shorten_transition(int state_index, int transition_index);

    void remove_long_transitions();

    static void simplify(FiniteAutomaton &automaton) {
        AutomatonSimplifier(automaton).remove_long_transitions();
    }

    FiniteAutomaton &automaton;
};