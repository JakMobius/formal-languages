cmake_minimum_required(VERSION 3.21)
project(formal_languages)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fsanitize=address")

file(GLOB_RECURSE ENGINE_FILES "${CMAKE_SOURCE_DIR}/src/engine/*.cpp" "${CMAKE_SOURCE_DIR}/src/engine/*.hpp")
file(GLOB_RECURSE TESTS_FILES "${CMAKE_SOURCE_DIR}/src/tests/*.cpp" "${CMAKE_SOURCE_DIR}/src/tests/*.hpp")
file(GLOB_RECURSE MAIN_FILES "${CMAKE_SOURCE_DIR}/src/main/*.cpp" "${CMAKE_SOURCE_DIR}/src/tests/*.hpp")

add_executable(formal_languages ${MAIN_FILES} ${ENGINE_FILES})
add_executable(formal_languages_tests ${TESTS_FILES} ${ENGINE_FILES} )

