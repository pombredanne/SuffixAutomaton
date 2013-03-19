#include "suffix_automaton.h"
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

int main()
{
    SuffixAutomaton sa("ababb");

    ofstream gv_file("viz/graph.gv");
    sa.make_gv( gv_file );

    return 0;
}

