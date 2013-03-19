#include "suffix_automaton.h"
#include <cstring>

#include <iostream>

uint SuffixAutomaton::add_node(uint len, uint link, uint endpos)
{
    uint size = _nodes.size();
    _nodes.resize( size + 1 );

    _nodes[size]._term   = false;
    _nodes[size]._endpos = endpos;
    _nodes[size]._len    = len;
    _nodes[size]._link   = link;

    std::memset( _nodes[size]._go, 0, sizeof(_nodes[size]._go) );

    return size;
}

SuffixAutomaton::SuffixAutomaton(std::string const& str)
    : _str(str),
      _last(1)
{
    _nodes.reserve( str.size() * 2 );

    add_node(0, 0, 0);     // t0, null node
    add_node(0, 0, 0);     // t1, initial state of automaton

    for (uint i = 0; i < str.size(); ++i)
    {
        // adding symbol s[i]
        uint c = str[i] - 'a';

        uint cur = add_node(_nodes[_last]._len + 1, 0, i);

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
                uint clone = add_node(_nodes[ p ]._len + 1, _nodes[ q ]._link, _nodes[ q ]._endpos);
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
}

void SuffixAutomaton::make_gv(std::ofstream& out, bool draw_suf_links)
{
    using namespace std;

    for (uint i = 1; i < _nodes.size(); ++i)
        cout << "nodes[" << i << "]: len = " << _nodes[i]._len << ", endpos = " << _nodes[i]._endpos
             << " str = \"" << _str.substr( _nodes[i]._endpos - _nodes[i]._len + 1, _nodes[i]._len ) << "\"\n";

    out << "digraph finite_state_machine {" << endl;
    out << "    rankdir=LR;" << endl;
    out << "    size=\"8,5\"" << endl << endl;

    out << "    node [shape = doublecircle];";

    for (uint i = 1; i < _nodes.size(); ++i)
        if ( _nodes[i]._term )
            out << " " << i;
    out << ";" << endl << endl;

    out << "    node [shape = circle];" << endl;

    for (uint i = 1; i < _nodes.size(); ++i)
    {
        for (uint c = 0; c < 26; ++c)
            if (_nodes[i]._go[c] != 0)
                out << "    " << i << " -> " << _nodes[i]._go[c] <<
                       " [ label = \"" << (char) (c + 'a') << "\", color=\"blue\" ];" << endl;

        if (draw_suf_links && _nodes[i]._link != 0)
            out << "    " << i << " -> " << _nodes[i]._link <<
                   " [ style=\"dashed\", constraint=\"false\" ];" << endl;

        out << endl;
    }

    out << "}" << endl;
}
