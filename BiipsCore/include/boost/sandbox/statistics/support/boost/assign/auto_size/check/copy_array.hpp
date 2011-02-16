//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_copy_array.hpp                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_COPY_ARRAY_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_COPY_ARRAY_ER_2010_HPP
#include <algorithm>
#include <boost/range.hpp>
#include <boost/assign/auto_size/check/constants.hpp>

#ifndef BOOST_ASSIGN_CHECK_EQUAL
#error
#endif

#define BOOST_ASSIGN_AS_CHECK_copy_array                                       \
{                                                                              \
    typedef T val_;                                                            \
    using namespace check_constants;                                           \
    val_ a1 = a, b1 = b, c1 = c,                                               \
         d1 = d, e1 = e, f1 = f;                                               \
    BOOST_AUTO(tmp1,BOOST_ASSIGN_AS_CHECK_ref3(a1,b1,c1));                     \
    BOOST_AUTO(tmp2,BOOST_ASSIGN_AS_CHECK_ref3(d1,e1,f1));                     \
                                                                               \
    tmp1.swap(tmp2);                                                           \
    BOOST_ASSIGN_CHECK_EQUAL( a , d1 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( b , e1 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( c , f1 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( d , a1 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( e , b1 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( f , c1 );                                        \
    BOOST_ASSIGN_AS_CHECK_ref3(a1,b1,c1).assign(0);                            \
    BOOST_ASSIGN_CHECK_EQUAL( a1 , 0 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( b1 , 0 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( c1 , 0 );                                        \
}                                                                              \
/**/

#endif
