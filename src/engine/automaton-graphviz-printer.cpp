
#include "automaton-graphviz-printer.hpp"

std::ostream &operator<<(std::ostream &stream, const AutomatonGraphvizPrinter &printer) {
    stream << "digraph finite_state_machine {" << std::endl;
    stream << "    rankdir=LR;" << std::endl;
    stream << "    size=\"8,5\"" << std::endl;
    stream << "    node [shape = doublecircle]; ";

    auto &states = printer.automaton.get_states();

    for (size_t i = 0; i < states.size(); i++) {
        if (states[i].is_final) {
            stream << i << " ";
        }
    }
    stream << ";" << std::endl;
    stream << "    node [shape = circle];" << std::endl;
    stream << "    start [label=\"\", shape = none];" << std::endl;
    stream << "    start -> " << printer.automaton.get_start_state_index() << ";" << std::endl;
    for (size_t i = 0; i < states.size(); i++) {
        for (auto &transition: states[i].transitions) {
            stream << "    " << i << " -> " << transition.target_index << " [ label = \"" << transition.regex
                   << "\" ];" << std::endl;
        }
    }
    stream << "}" << std::endl;
    return stream;
}
