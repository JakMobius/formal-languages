
#include "earley-parser.hpp"

bool EarleyParser::parse(const std::string &input) {
    str = input;
    states = std::vector<std::unordered_set<EarleyState>>(str.size() + 1);

    std::unordered_set<EarleyState> next_states;
    std::unordered_set<EarleyState> next_states_completed;

    std::unordered_set<EarleyState> start_rule;
    std::string fake_start_nonterminal = free_nonterminal_name(grammar, "S");
    GrammarRule start_rule_grammar = GrammarRule({NonTerminal{fake_start_nonterminal}},
                                                 {grammar.get_start_nonterminal()});

    start_rule.insert({start_rule_grammar, 0, 0});
    states[0] = start_rule;

    for (int i = 0; i <= str.size(); ++i) {
        next_states = scan(i);
        next_states_completed.clear();

        for (auto &rule: next_states) {
            states[i].insert(rule);
        }

        for(int j = 0, changed = 1; changed; j++) {
            changed = 0;
            if (j == 0 || next_states_completed.empty()) {
                next_states_completed = complete(states[i]);
            } else {
                next_states_completed = complete(next_states_completed);
            }
            for (auto &rule: next_states_completed) {
                if (!find_state(i, rule)) {
                    states[i].insert(rule);
                    changed = 1;
                }
            }
            next_states = predict(i);
            for (auto &rule: next_states) {
                if (!find_state(i, rule)) {
                    states[i].insert(rule);
                    changed = 1;
                }
            }
        }
    }

    EarleyState finish_rule = {start_rule_grammar, 0, 1};

    return find_state(str.size(), finish_rule);
}

std::unordered_set<EarleyState> EarleyParser::scan(int index) {
    std::unordered_set<EarleyState> find_sets;
    if (index == 0) {
        return find_sets;
    }
    for (auto &situation: states[index - 1]) {
        if (situation.dot_position < situation.rule.right.size()) {
            auto symbol = situation.rule.right[situation.dot_position];
            if (symbol == Terminal{str[index - 1]}) {
                EarleyState new_rule = situation;
                ++new_rule.dot_position;
                find_sets.insert(new_rule);
            }
        }
    }
    return find_sets;
}

std::unordered_set<EarleyState> EarleyParser::predict(int index) {
    std::unordered_set<EarleyState> find_sets;
    for (auto &situation: states[index]) {
        if (situation.dot_position < situation.rule.right.size()) {
            auto symbol = situation.rule.right[situation.dot_position];
            if (symbol.type == SymbolType::non_terminal) {
                for (auto &grammar_rule: grammar.get_rules()) {
                    if (grammar_rule.left[0] == symbol) {
                        EarleyState new_rule = {grammar_rule, index, 0};
                        find_sets.insert(new_rule);
                    }
                }
            }
        }
    }
    return find_sets;
}

std::unordered_set<EarleyState> EarleyParser::complete(std::unordered_set<EarleyState> &new_situations) {
    std::unordered_set<EarleyState> find_sets;
    for (auto &situation: new_situations) {
        if (situation.dot_position == situation.rule.right.size()) {
            int parent_id = situation.index_parent;
            auto left = situation.rule.left[0];
            for (auto &complete_situation: states[parent_id]) {
                if (complete_situation.dot_position < complete_situation.rule.right.size()) {
                    if (complete_situation.rule.right[complete_situation.dot_position] == left) {
                        EarleyState new_rule = complete_situation;
                        new_rule.dot_position++;
                        find_sets.insert(new_rule);
                    }
                }
            }
        }
    }
    return find_sets;
}

bool EarleyState::operator==(const EarleyState &other) const {
    return rule == other.rule && index_parent == other.index_parent && dot_position == other.dot_position;
}
