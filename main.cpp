#include "suffix_automaton.h"
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "Usage: " << endl;
        cout << "   " << argv[0] << " \"some string\"" << endl;
    }
    else
    {
        SuffixAutomaton sa( argv[1] );

        // Generate Graphviz input file in DOT language
        ofstream gv_file("graph.gv");
        sa.make_gv( gv_file, true );
    }

    return 0;
}

