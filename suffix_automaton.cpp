#include "suffix_automaton.h"
#include <cstring>
#include <cstdio>
#include <iostream>

uint SuffixAutomaton::add_node(uint len, uint link, uint endpos, uint cnt)
{
    uint size = _nodes.size();
    _nodes.resize( size + 1 );

    _nodes[size]._term   = false;
    _nodes[size]._endpos = endpos;
    _nodes[size]._len    = len;
    _nodes[size]._link   = link;
    _nodes[size]._cnt    = cnt;

    std::memset( _nodes[size]._go, 0, sizeof(_nodes[size]._go) );

    return size;
}

void SuffixAutomaton::walk_lexicographically(uint node)
{
    if ( _nodes[node]._term && node != 1 )
    {
        std::cout << "\"" << _cur_str << "\"" << std::endl;
    }

    for (uint c = 0; c < 26; ++c)
        if ( _nodes[node]._go[c] != 0 )
        {
            _cur_str.push_back( c + 'a' );
            walk_lexicographically( _nodes[node]._go[c] );
            _cur_str.resize( _cur_str.size() - 1 );
        }
}

void SuffixAutomaton::print_suffixes()
{
    _cur_str = "";
    walk_lexicographically( 1 );
}

void SuffixAutomaton::reset_was()
{
    for (uint i = 0; i < _nodes.size(); ++i)
        _nodes[i]._was = false;
}

SuffixAutomaton::SuffixAutomaton(std::string const& str)
    : _str(str),
      _last(1)
{
    _nodes.reserve( str.size() * 2 );

    add_node(0, 0, 0, 0);     // t0, null node
    add_node(0, 0, 0, 0);     // t1, initial state of automaton

    for (uint i = 0; i < str.size(); ++i)
    {
        // adding symbol s[i]
        uint c = str[i] - 'a';

        uint cur = add_node(_nodes[_last]._len + 1, 0, i, 1);

        uint p;
        for (p = _last; (p != 0) && (_nodes[p]._go[str[i] - 'a'] == 0); p = _nodes[p]._link)
            _nodes[p]._go[ c ] = cur;

        if (p != 0)
        {
            uint q = _nodes[p]._go[ c ];

            if (_nodes[p]._len + 1 == _nodes[q]._len)
                _nodes[cur]._link = q;
            else
            {
                uint clone = add_node(_nodes[ p ]._len + 1, _nodes[ q ]._link, _nodes[ q ]._endpos, 0);
                std::memcpy(_nodes[clone]._go, _nodes[q]._go, sizeof(_nodes[q]._go));

                for (; (p != 0) && (_nodes[p]._go[ c ] == q); p = _nodes[p]._link)
                    _nodes[p]._go[ c ] = clone;

                _nodes[q]._link   = clone;
                _nodes[cur]._link = clone;
            }
        }
        else
            _nodes[cur]._link = 1;

        _last = cur;
    }

    // Mark terminating nodes
    for (uint t = _last; t != 0; t = _nodes[t]._link)
        _nodes[t]._term = true;

    reset_was();
    build_cnt(1);
}

void SuffixAutomaton::build_cnt(uint node)
{
    if ( _nodes[node]._was )
        return;
    _nodes[node]._was = true;

    for (uint c = 0; c < 26; ++c)
        if (_nodes[node]._go[c] != 0)
            build_cnt( _nodes[node]._go[c] );

    _nodes[ _nodes[node]._link ]._cnt += _nodes[node]._cnt;
}

void SuffixTree::dfs(uint node)
{
    for (uint c = 0; c < 26; ++c)
    {
        uint dest = _nodes[node]._go[c];
        if ( dest != 0 )
        {
            printf("%u %u %u %u\n", node, dest, _nodes[dest]._l + 1, _nodes[dest]._r + 1);
            dfs( dest );
        }
    }
}

SuffixTree::SuffixTree(SuffixAutomaton const& sa)
    : _nodes( sa._nodes.size() )
{
    for (uint i = 0; i < _nodes.size(); ++i)
        std::memset( _nodes[i]._go, 0, sizeof(_nodes[i]._go) );

    for (uint i = 2; i < sa._nodes.size(); ++i)
    {
        uint parent = sa._nodes[i]._link;

        // Finding proper label for this edge
        uint len = sa._nodes[i]._len;
        uint endpos = sa._nodes[i]._endpos;
        uint start = (endpos + 1) - len;
        uint end = endpos - sa._nodes[parent]._len;

        _nodes[i]._l = sa._str.size() - end - 1;
        _nodes[i]._r = sa._str.size() - start - 1;

        _nodes[parent]._go[ sa._str[ end ] - 'a' ] = i;

        std::cout << parent << " --" << (sa._str[ end ]) << "--> " << i << "  (" << _nodes[i]._l + 1 << ", " << _nodes[i]._r + 1 << ")\n";
    }
}

void SuffixTree::print_tree()
{
    printf("%u %u\n", _nodes.size() - 1, _nodes.size() - 2);
    dfs(1);
}

void SuffixAutomaton::make_gv(std::ofstream& out, bool draw_suf_links)
{
    using namespace std;

    out << "digraph finite_state_machine\n{\n";

    out << "    /* Draw graph from left to right */\n";
    out << "    rankdir = LR;\n\n";

    out << "    /* Automaton states (graph nodes) */\n";
    out << "    node [ shape = \"circle\" ]\n";

    for (uint i = 1; i < _nodes.size(); ++i)
    {
        uint endpos  = _nodes[i]._endpos;
        uint len     = _nodes[i]._len;
        uint linklen = _nodes[ _nodes[i]._link ]._len + 1;

        out << "      " << i << "  [ label = \"" << _str.substr( endpos - len + 1, len );
        if ( linklen < len )
            out << "\\n...\\n" << _str.substr( endpos - linklen + 1, linklen );
        out << "\"";

        // Some information about substrings. Disabled because looked ugly.
        //out << "\\ncnt = " << _nodes[i]._cnt << "\\ni = " << i << "\\nendpos = " << _nodes[i]._endpos;
        //out << "\"";

        if ( _nodes[i]._term )
            out << ", shape = \"doublecircle\"";

        out << " ]\n";
    }
    out << "\n";

    out << "    /* Edges */\n";
    out << "    edge [ color = \"blue\", fontcolor = \"blue\" ]\n";
    for (uint i = 1; i < _nodes.size(); ++i)
    {
        for (uint c = 0; c < 26; ++c)
            if (_nodes[i]._go[c] != 0)
                out << "      " << i << " -> " << _nodes[i]._go[c] <<
                       " [ label = \"" << (char) (c + 'a') << "\" ]\n";
    }

    if ( draw_suf_links )
    {
        out << "\n";
        out << "    /* Fallback edges (a.k.a. suffix links) */\n";
        out << "    edge [ style = \"dashed\", color = \"darkgreen\", constraint = \"false\" ]\n";

        for (uint i = 2; i < _nodes.size(); ++i)
            out << "      " << i << " -> " << _nodes[i]._link << "\n";
    }

    out << "    \n    label = \"String: " << _str << "\"\n";
    out << "    fontsize = 12\n";

    out << "}" << endl;
}
