#ifndef CSET_HPP
#define CSET_HPP

#include <unordered_map>

#include "Tidset.hpp"
#include "Itemset.hpp"

typedef Tidset cset_key_t;

//typedef std::pair< Itemset, unsigned int > cset_val_t;
typedef Itemset cset_val_t;

typedef std::unordered_multimap< cset_key_t, cset_val_t, tidset_hash > CSet;

inline std::ostream & operator << ( std::ostream & os, const CSet & c_set )
{
    std::for_each( c_set.cbegin(), c_set.cend(), [&]( CSet::const_reference entries ) {
        os << entries.second << ' ' /*<< entries.first << ' '*/ << entries.first.size() << '\n';
    } );

    return os;
}


#endif // CSET_HPP
