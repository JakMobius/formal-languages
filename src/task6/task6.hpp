#pragma once

#include <string>
#include <vector>
#include <stack>
#include "../engine/regex.hpp"
#include "../engine/finite-automaton.hpp"
#include "../engine/automaton-simplifier.hpp"
#include "../engine/epsilon-remover.hpp"
#include "../engine/automaton-completer.hpp"
#include "../engine/automaton-minifier.hpp"
#include "../engine/automaton-determinator.hpp"

/*
 * Задача 6:
 *
 * Входные данные:
 * - регулярное выражение, заданное в виде строки в обратной польской записи
 * - символ ch
 * - число k
 *
 * Выходные данные:
 * - определить, принимает ли автомат, построенный по данному регулярному выражению, слово с суффиксом ch^k,
 * и если принимает, то вывести минимальную длину такого слова.
 *
 * Алгоритм решения:
 * 1. Прочитать регулярное выражение из строки в обратной польской записи
 *
 * 2. Инвертировать регулярное выражение (развернуть слова из языка, принимаемого регулярным выражением)
 *  - *: Теперь задача сводится к поиску наикратчайшего слова с префиксом ch^k
 *
 * 3. Построить ПДКА по данному регулярному выражению.
 *
 * 4. Перейти в ПДКА k раз по символу ch.
 *
 * 5. Найти кратчайший путь от текущего состояния до финального, и сложить это число с k - это и будет ответом.
 *
 * 6. Если пути нет, то ответ -1.
 *
 * Доказательство корректности:
 * Пусть в языке L заданного регулярным выражением есть наикратчайшее слово u' с суффиксом ch^k.
 * Тогда в языке, состоящем из развёрнутых слов есть наикратчайшее слово v' = rev(v') c префиксом ch^k.
 * Иными словами, v' = ch^k * m. Тогда, перейдя в ПДКА k раз по символу ch, мы попадём в состояние, из которого
 * финальное состояние достижимо по слову m. Таким образом, ответом алгоритма будет k + |m| = |rev(v')| = |u'|.
 *
 * Пусть в языке L заданного регулярным выражением нет слова с суффиксом ch^k. Тогда в языке, состоящем из развёрнутых слов
 * нет слова с префиксом ch^k. Таким образом, перейдя в ПДКА k раз по символу ch, мы попадём в состояние, из которого
 * финальное состояние не достижимо. Таким образом, ответом алгоритма будет -1.
 *
 * Асимптотика:
 * 1. Чтение регулярного выражения из строки в обратной польской записи: O(|s|)
 * 2. Инвертирование регулярного выражения: O(|s|)
 * 3. Построение ПДКА по данному регулярному выражению: O(2^|s|)
 * 4. Переход в ПДКА k раз по символу ch: O(k)
 * 5. Поиск кратчайшего пути в ПДКА: O(2^|s|)
 *
 * Итого: O(2^|s|)
 */

namespace Task6 {

    Regex parse_regex_invert_polish_notation(const std::string &regex) {
        std::stack<Regex> stack;
        for (char i: regex) {
            switch (i) {
                case 'a':
                case 'b':
                case 'c':
                    stack.push(Regex(CharRegex(i)));
                    break;
                case '*': {
                    Regex r1 = stack.top();
                    stack.pop();
                    Regex r2 = stack.top();
                    stack.pop();
                    stack.push(r1 + r2);
                    break;
                }
                case '1': {
                    stack.push(Regex());
                }
                case '.': {
                    Regex r3 = stack.top();
                    stack.pop();
                    Regex r4 = stack.top();
                    stack.pop();
                    stack.push(r3 * r4);
                    break;
                }
                default:
                    break;
            }
        }
        return stack.top();
    }

    int reach_final_state(FiniteAutomaton &automaton, size_t start_index) {
        if (automaton.get_states()[start_index].is_final) {
            return 0;
        }

        std::set<size_t> current_states;
        current_states.insert(start_index);
        int steps = 0;
        size_t old_states_size = 0;

        while(current_states.size() != old_states_size) {
            steps++;
            std::set<size_t> next_states;
            for (auto state_index: current_states) {
                auto &state = automaton.get_states()[state_index];
                for (auto &transition: state.transitions) {
                    if (automaton.get_states()[transition.target_index].is_final) {
                        return steps;
                    }
                    next_states.insert(transition.target_index);
                }
            }
            old_states_size = current_states.size();
            current_states.insert(next_states.begin(), next_states.end());
        };

        return -1;
    }

    Regex inverse_regex(const Regex &regex) {
        switch (regex.type) {
            case RegexType::Char:
            case RegexType::Star:
            case RegexType::Sum:
                return regex;
            case RegexType::Concat: {
                auto &value = std::get<ConcatRegex>(regex.value);
                ConcatRegex reversed_regex;
                for (int i = value.operands.size() - 1; i >= 0; i--) {
                    reversed_regex.operands.push_back(inverse_regex(value.operands[i]));
                }
                return reversed_regex;
            }
        }
    }

    int solve(const std::string &input_string, char ch, int k) {
        Regex regex = inverse_regex(parse_regex_invert_polish_notation(input_string));

        FiniteAutomaton automaton(regex);
        automaton.extend_alphabet({'a', 'b', 'c'});
        AutomatonSimplifier(automaton).simplify();
        EpsilonRemover(automaton).simplify();
        AutomatonCompleter(automaton).complete();
        automaton = AutomatonDeterminator(automaton).determine();

        size_t current_state = automaton.get_start_state_index();

        int word_size = 0;

        for (int i = 0; i < k; i++) {
            int transition = automaton.find_transition(ch, current_state);
            current_state = automaton.get_transition(current_state, transition).target_index;
            word_size++;
        }

        // Determine whether the final state is reachable from the current state

        int steps = reach_final_state(automaton, current_state);

        if (steps == -1) {
            return -1;
        }

        return steps + word_size;
    }

};