//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_example2.hpp                            //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_EXAMPLE2_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_EXAMPLE2_ER_2010_HPP
#include <vector>
#include <iterator>
#include <boost/range/algorithm/copy.hpp>
#include <boost/assign/auto_size/range/chain_r.hpp>

#define BOOST_ASSIGN_AS_CHECK_example2                                         \
{                                                                              \
    typedef T val_;                                                            \
    std::vector<int> v, v2;                                                    \
    v  = BOOST_ASSIGN_AS_CHECK_cref3(1,2,3);                                   \
    boost::copy(                                                               \
        BOOST_ASSIGN_AS_CHECK_cref1(0) && v && v && BOOST_ASSIGN_AS_CHECK_cref1(4),\
        std::back_inserter(v2)                                                 \
    );                                                                         \
    BOOST_ASSIGN_CHECK_EQUAL( v2.size() , 8u );                                \
   boost::copy(                                                                \
       v2 && BOOST_ASSIGN_AS_CHECK_cref1(5),                                   \
       std::back_inserter(v)                                                   \
    );                                                                         \
    BOOST_ASSIGN_CHECK_EQUAL( v.size() , 12u );                                \
    int x = 0;                                                                 \
    int y = 1;                                                                 \
    BOOST_ASSIGN_CHECK_EQUAL(                                                  \
        boost::size(                                                           \
            BOOST_ASSIGN_AS_CHECK_cref1( x ) && v2 && BOOST_ASSIGN_AS_CHECK_cref1( y )\
        ),                                                                     \
        10u                                                                    \
    );                                                                         \
}                                                                              \
/**/

#endif
