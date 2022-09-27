cmake_minimum_required(VERSION 3.21)
project(formal_languages)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -fsanitize=address)

add_executable(formal_languages
        src/main.cpp
        src/regex.cpp
        src/regex.hpp
        src/finite-automaton.cpp
        src/finite-automaton.hpp
        src/automaton-graphviz-printer.cpp
        src/automaton-graphviz-printer.hpp
        src/automaton-simplifier.cpp
        src/automaton-simplifier.hpp
        src/automaton-collapser.cpp
        src/automaton-collapser.hpp)

