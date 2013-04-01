#ifndef SUFFIX_AUTOMATON_H
#define SUFFIX_AUTOMATON_H

#include <vector>
#include <string>
#include <fstream>

typedef unsigned int uint;

class SuffixAutomaton;

// You can also construct suffix tree from the automaton
class SuffixTree
{
private:
    struct Node
    {
        uint _l, _r; // _l and _r belong to this node's edge
        uint _go[26];
    };

    std::vector<Node> _nodes;

    void dfs(uint node);
    SuffixTree();

public:
    SuffixTree(SuffixAutomaton const& sa);
    void print_tree();
};

class SuffixAutomaton
{
    friend SuffixTree::SuffixTree(SuffixAutomaton const& sa);

private:
    struct Node
    {
        uint _len;
        uint _endpos;
        uint _cnt;

        uint _go[26];
        uint _link;

        bool _was;
        bool _term;
    };

    std::string _str;
    std::vector<Node> _nodes;
    uint _last;

    uint add_node(uint len, uint link, uint endpos, uint cnt);

    std::string _cur_str;
    void walk_lexicographically(uint node);

    void reset_was();
    void build_cnt(uint node);

    SuffixAutomaton();

public:
    SuffixAutomaton(std::string const& str);

    void print_suffixes();

    void make_gv(std::ofstream& out, bool draw_suf_links = false);
};

#endif // SUFFIX_AUTOMATON_H
