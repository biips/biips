//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_example1.hpp                            //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_EXAMPLE1_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_EXAMPLE1_ER_2010_HPP
#include <iostream>
#include <boost/range/algorithm/max_element.hpp>

#define BOOST_ASSIGN_AS_CHECK_example1                                         \
{                                                                              \
    typedef T val_;                                                            \
    using namespace check_constants;                                           \
    val_ a1 = a, b1 = b, c1 = c, d1 = d,                                       \
         e1 = e, f1 = f, g1 = g, h1 = h;                                       \
                                                                               \
    val_& max = *boost::max_element(                                           \
        BOOST_ASSIGN_AS_CHECK_ref8(a1, b1, c1, d1, e1, f1, g1, h1) );          \
    BOOST_ASSIGN_CHECK_EQUAL( max , f1 );                                      \
    max = 8;                                                                   \
    BOOST_ASSIGN_CHECK_EQUAL( f1 , 8 );                                        \
    const val_& const_max = *boost::max_element(                               \
        BOOST_ASSIGN_AS_CHECK_cref8(a, b, c, d1, e1, f1, g1, h1) );            \
    BOOST_ASSIGN_CHECK_EQUAL( max , const_max );                               \
                                                                               \
}                                                                              \
/**/

#endif
