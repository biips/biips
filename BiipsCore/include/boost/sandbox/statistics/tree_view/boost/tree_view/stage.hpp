///////////////////////////////////////////////////////////////////////////////
// tree_view::stage.hpp                                                      //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_TREE_VIEW_STAGE_HPP_ER_2009
#define BOOST_TREE_VIEW_STAGE_HPP_ER_2009
#include <stdexcept>
#include <boost/format.hpp>

namespace boost{
namespace tree_view{

    // This class maps a position in a tree structure to a position in a vector
    //
    // The tree structure has a root node (stage 0) with n adjacent 
    // nodes (stage 1), each of which have n adjacent nodes (stage 2) etc.
    // The nodes are stored in a vector, starting with the root node,
    // followed by those in stage1, then those in stage 2 etc. (breadth first)
    
    // j : stage
    // n : number of branches per node
    template<unsigned j,unsigned n>
    struct stage{
        static unsigned position_first;
        static unsigned position_last;
        static unsigned number_nodes;
    };

    template<unsigned n>
    struct stage<0,n>{
        static unsigned position_first;
        static unsigned position_last;
        static unsigned number_nodes;
    };


    // Client may ignore this:
    template<typename T> struct position_first_{ static unsigned get(); };
    template<typename T> struct position_last_{ static unsigned get(); };
    template<typename T> struct number_nodes_{ static unsigned get(); };

    // Implementation //

    // Initialization
    template<unsigned n>
    unsigned stage<0,n>::position_first = 0;

    template<unsigned n>
    unsigned stage<0,n>::position_last = 1;

    template<unsigned n>
    unsigned stage<0,n>::number_nodes = 1;

    // Recursion
    template<unsigned j,unsigned n>
    unsigned stage<j,n>::position_first 
        = stage<j-1,n>::position_last;

    template<unsigned j,unsigned n>
    unsigned stage<j,n>::position_last 
        = stage<j,n>::position_first 
            + stage<j,n>::number_nodes;

    template<unsigned j,unsigned n>
    unsigned stage<j,n>::number_nodes 
        = stage<j-1,n>::number_nodes * n;

    // T = stage
    template<typename T>
    unsigned position_first_<T>::get(){ return T::position_first; }
    template<typename T>
    unsigned position_last_<T>::get(){ return T::position_last; }
    template<typename T>
    unsigned number_nodes_<T>::get(){ return T::number_nodes; }

}// tree_view
}// boost

#endif