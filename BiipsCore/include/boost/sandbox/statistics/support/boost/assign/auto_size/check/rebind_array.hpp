//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_rebind_array.hpp                        //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_REBIND_ARRAY_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_REBIND_ARRAY_ER_2010_HPP
#include <boost/assign/auto_size/check/constants.hpp>

#ifndef BOOST_ASSIGN_CHECK_EQUAL
#error
#endif

#define BOOST_ASSIGN_AS_CHECK_rebind_array                                     \
{                                                                              \
    typedef T val_;                                                            \
    using namespace check_constants;                                           \
    val_ a1 = a, b1 = b, c1 = c,                                               \
         d1 = d, e1 = e, f1 = f;                                               \
    BOOST_AUTO(tmp1,BOOST_ASSIGN_AS_CHECK_ref_rebind3(a1, b1, c1));            \
    BOOST_AUTO(tmp2,BOOST_ASSIGN_AS_CHECK_ref_rebind3(d1, e1, f1));            \
    tmp1.swap(tmp2);                                                           \
    BOOST_ASSIGN_CHECK_EQUAL( tmp1[0] , d );                                   \
    BOOST_ASSIGN_CHECK_EQUAL( tmp1[1] , e );                                   \
    BOOST_ASSIGN_CHECK_EQUAL( tmp1[2] , f );                                   \
    BOOST_ASSIGN_CHECK_EQUAL( tmp2[0] , a );                                   \
    BOOST_ASSIGN_CHECK_EQUAL( tmp2[1] , b );                                   \
    BOOST_ASSIGN_CHECK_EQUAL( tmp2[2] , c );                                   \
    BOOST_ASSIGN_CHECK_EQUAL( a , a1 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( b , b1 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( c , c1 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( d , d1 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( e , e1 );                                        \
    BOOST_ASSIGN_CHECK_EQUAL( f , f1 );                                        \
}                                                                              \
/**/

#endif
