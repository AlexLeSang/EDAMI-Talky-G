#ifndef CHARM_HPP
#define CHARM_HPP

#include "CSet.hpp"

#include "Database.hpp"
#include "Node.hpp"
#include <cassert>
#include <chrono>

/*!
 * \brief ItemMap
 */
typedef std::unordered_map< Item, Tidset, item_hash > ItemMap;

namespace Talky_G
{

/*!
 * \brief is_subsumed
 * \param c_set
 * \param Z_itemset
 * \param Z_tidset
 * \return
 */
inline bool is_subsumed(const CSet &c_set, const Itemset & Z_itemset, const Tidset & Z_tidset )
{
    //    std::cerr << "subsumption: Z_itemset: " << Z_itemset << " Z_tidset: " << Z_tidset << std::endl;
    bool is_subsumed = false;
    const auto range = c_set.equal_range( Z_tidset );
    for ( auto it = range.first; it != range.second; ++ it ) {
        const Tidset & G_tidset = (*it).first;
        if ( G_tidset.size() == Z_tidset.size() ) {
            const Itemset & G_itemset = (*it).second;
            // Check if Z is a superset of G
            if ( std::includes( Z_itemset.cbegin(), Z_itemset.cend(), G_itemset.cbegin(), G_itemset.cend() ) ) {
                is_subsumed = true;
                break;
            }
        }
    }
    return is_subsumed;
}

/*!
 * \brief is_subsumed
 * \param c_set
 * \param node
 * \return
 */
inline bool is_subsumed(const CSet &c_set, const Node & node)
{
    return is_subsumed( c_set, node.itemset(), node.tidset() );
}

/*!
 * \brief itemset_union
 * \param itemset_l
 * \param itemset_r
 * \return
 */
inline Itemset itemset_union(const Itemset &itemset_l, const Itemset & itemset_r)
{
    Itemset union_itemset( itemset_l.size() + itemset_r.size() );
    auto it_union = std::set_union( itemset_l.cbegin(), itemset_l.cend(), itemset_r.cbegin(), itemset_r.cend(), union_itemset.begin() );
    union_itemset.resize( std::distance(union_itemset.begin(), it_union) );
    return std::move(union_itemset);
}

/*!
 * \brief tidset_intersection
 * \param tidset_l
 * \param tidset_r
 * \return
 */
inline Tidset tidset_intersection(const Tidset &tidset_l, const Tidset &tidset_r)
{
    Tidset resutl_tidset( tidset_l.size() + tidset_r.size() );
    const auto it = std::set_intersection( tidset_r.cbegin(), tidset_r.cend(), tidset_l.cbegin(), tidset_l.cend(), resutl_tidset.begin() );
    resutl_tidset.resize( std::distance( resutl_tidset.begin(), it ) );
    return std::move(resutl_tidset);
}

/*!
 * \brief save
 * \param c_set
 * \param child
 */
inline void save(CSet & c_set, const Node & child) {
    //    std::cerr << "save: " << child << std::endl;
    c_set.insert( CSet::value_type( child.tidset(), child.itemset() ) );
}

/*!
 * \brief is_null
 * \param node
 * \return
 */
inline bool is_null(const Node & node)
{
    if ( node.tidset().empty() || node.tidset().empty() ) {
        return true;
    }
    else {
        return false;
    }
}

/*!
 * \brief get_next_generator
 * \param curr
 * \param other
 * \param c_set
 * \param min_sup
 * \return
 */
inline Node get_next_generator(const Node & curr, const Node & other, const CSet & c_set, const unsigned int min_sup)
{
    Tidset cand_tidset = tidset_intersection( curr.tidset(), other.tidset() );
    //    std::cerr << "cand_tidset: " << cand_tidset << std::endl;
    // Check support
    if ( cand_tidset.size() < min_sup ) {
        //        std::cerr << "Infrequent\n" << std::endl;
        return Node(); // Return null
    }
    // Check equality
    if ( std::equal( curr.tidset().cbegin(), curr.tidset().cend(), cand_tidset.cbegin() ) ||
         std::equal( other.tidset().cbegin(), other.tidset().cend(), cand_tidset.cbegin() ) )
    {
        //        std::cerr << "Not generator\n" << std::endl;
        return Node(); // Return null
    }
    Itemset cand_itemset = itemset_union( curr.itemset(), other.itemset() );
    //    std::cerr << "cand_itemset: " << cand_itemset << std::endl;
    if ( is_subsumed( c_set, cand_itemset, cand_tidset ) ) {
        //        std::cerr << "Subsumed\n" << std::endl;
        return Node();
    }
    return Node( cand_itemset, cand_tidset );
}


template< typename node_iterator >
/*!
 * \brief talky_g_extend
 * \param curr
 * \param right_margin
 * \param c_set
 * \param min_sup
 */
inline void talky_g_extend(node_iterator & curr, const node_iterator & right_margin, CSet &c_set, const unsigned int min_sup)
{
    Node & current_child = (*(*curr));
    //    std::cerr << "\ncurrent_child: " << current_child << std::endl;
    // Loop over the right siblings of curr from Left to Right
    //    std::cerr << "std::distance( curr, right_margin ) = " << std::distance( right_margin, curr ) << std::endl;
    if ( std::distance( right_margin, curr ) >= 1 ) {
        for ( auto it = curr - 1; std::distance( right_margin, it ) >= 0; --it ) {
            //            std::cerr << "std::distance( right_margin, it ) = " << std::distance( right_margin, it ) << std::endl;
            const Node & other = (*(*it));
            //            std::cerr << "other: " << other << std::endl;
            const auto generator = get_next_generator( current_child, other, c_set, min_sup );
            if ( ! is_null( generator ) ) {
                current_child.add_child( generator );
            }
        }
        // Loop over the children of curr from Left to Right
        for ( auto it = current_child.children().crbegin(); it != current_child.children().crend(); ++ it ) {
            const Node & child = (*(*it));
            save( c_set, child );
            talky_g_extend( it, current_child.children().crbegin(), c_set, min_sup );
        }
    }
    else {
        //        std::cerr << "No right siblings" << std::endl;
    }
}

/*!
 * \brief talky_g
 * \param database
 * \param min_sup
 * \return
 */
inline CSet talky_g( const Database & database, const unsigned int min_sup ) {
    Node p;
    // Create a P tree
    {
        ItemMap item_map;
        // For each itemset
        TID transaction_counter = 1;
        std::for_each( database.cbegin(), database.cend(), [&] ( const Itemset & itemset ) {
            // For each item in itemset
            std::for_each( itemset.cbegin(), itemset.cend(), [&]( const Item & item ) {
                // If there is such item increase support
                const auto got = item_map.find( item );
                if ( item_map.cend() != got ) {
                    got->second.push_back( transaction_counter );
                }
                // Else add item with sup == 1
                else {
                    Tidset new_tidset( 1 );
                    new_tidset.at( 0 ) = transaction_counter;
                    item_map.insert( std::make_pair( item, new_tidset ) );
                }
            } );

            ++ transaction_counter;
        } );

        // Fill the tree
        {
            std::for_each( item_map.cbegin(), item_map.cend(), [&]( ItemMap::const_reference key_value ) {
                if ( min_sup <= key_value.second.size() ) {
                    Itemset itemset;
                    itemset.push_back( key_value.first );
                    p.add_child( Node( itemset, key_value.second ) );
                }
            } );
        }

    }

    auto c_set = CSet();
    // Loop over children of root Right to Left
    for ( auto it = p.children().crbegin(); it != p.children().crend(); ++ it ) {
        Node & current_child = (*(*it));
        //        std::cerr << "root child: " << current_child << std::endl;
        save( c_set, current_child );
        talky_g_extend( it, p.children().crbegin(), c_set, min_sup );
    }

    //    std::cerr << "c_set: \n" << c_set << std::endl;
    return c_set;
}
}

#endif // CHARM_HPP
