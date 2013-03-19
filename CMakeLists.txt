project(suffix_automaton)
cmake_minimum_required(VERSION 2.8)

set(SRC_LIST main.cpp suffix_automaton.cpp)
add_executable(${PROJECT_NAME} ${SRC_LIST})

