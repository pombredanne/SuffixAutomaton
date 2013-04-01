project(suffix_automaton)
cmake_minimum_required(VERSION 2.8)

# Target: test program
set(SRC_LIST main.cpp suffix_automaton.cpp)
add_executable(string_to_gv ${SRC_LIST})

# Target: graph.gv file
add_custom_command(
        OUTPUT graph.gv
        COMMAND ./string_to_gv "test"
        DEPENDS string_to_gv
        VERBATIM
)

# Target: graph.png file
add_custom_command(
        OUTPUT graph.png
        COMMAND dot graph.gv -Tpng > graph.png
        DEPENDS graph.gv
        VERBATIM
)

# Needed just to build two targets above
add_custom_target(image ALL DEPENDS graph.png)
