#ifndef SUFFIX_AUTOMATON_H
#define SUFFIX_AUTOMATON_H

#include <vector>
#include <string>
#include <fstream>

typedef unsigned int uint;

class SuffixAutomaton
{
private:
    struct Node
    {
        uint _len;
        uint _endpos;

        uint _go[26];
        uint _link;

        bool _term;
    };

    std::string _str;
    std::vector<Node> _nodes;
    uint _last;

    uint add_node(uint len, uint link, uint endpos);

    SuffixAutomaton();

public:
    SuffixAutomaton(std::string const& str);

    void make_gv(std::ofstream& out, bool draw_suf_links = false);
};

#endif // SUFFIX_AUTOMATON_H
