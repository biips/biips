//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_chain_r.hpp                             //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_CHAIN_R_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_CHAIN_R_ER_2010_HPP
#include <boost/array.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/assign/auto_size/range/chain_common.hpp>
#include <boost/assign/auto_size/range/chain_r.hpp>
#include <boost/assign/auto_size/check/iterator.hpp>
#include <boost/assign/auto_size/check/constants.hpp>
#include <boost/assign/auto_size/check/copy_iterator.hpp>

#ifndef BOOST_ASSIGN_CHECK_EQUAL
#error
#endif

#define BOOST_ASSIGN_AS_CHECK_chain_r                                          \
{                                                                              \
    typedef T val_;                                                            \
    using namespace check_constants;                                           \
    val_ a1 = a, b1 = b, c1 = c,                                               \
         d1 = d, e1 = e, f1 = f,                                               \
         g1 = g, h1 = h;                                                       \
    typedef boost::array<T,2> ar2_;                                            \
    typedef boost::array<T,3> ar3_;                                            \
    ar3_ ar1, ar2;                                                             \
    ar2_ ar3;                                                                  \
    ar1[0] = a1; ar1[1] = b1; ar1[2] = c1;                                     \
    ar2[0] = d1; ar2[1] = e1; ar2[2] = f1;                                     \
    ar3[0] = g1; ar3[1] = h1;                                                  \
    BOOST_AUTO(tmp1,BOOST_ASSIGN_AS_CHECK_ref3(a1,b1,c1));                     \
    BOOST_AUTO(tmp2,BOOST_ASSIGN_AS_CHECK_ref3(d1,e1,f1));                     \
    BOOST_AUTO(tmp3,BOOST_ASSIGN_AS_CHECK_ref2(g1,h1));                        \
    check_iterator_impl( boost::begin( chain_r(ar1)(ar2)(ar3) ) );             \
    check_iterator_impl( boost::begin( chain_r(ar1)(ar2)(tmp3) ) );            \
    check_iterator_impl( boost::begin( chain_r(ar1)(tmp2)(ar3) ) );            \
    check_iterator_impl( boost::begin( chain_r(ar1)(tmp2)(tmp3) ) );           \
    check_iterator_impl( boost::begin( chain_r(tmp1)(ar2)(ar3) ) );            \
    check_iterator_impl( boost::begin( chain_r(tmp1)(ar2)(tmp3) ) );           \
    check_iterator_impl( boost::begin( chain_r(tmp1)(tmp2)(ar3) ) );           \
    check_iterator_impl( boost::begin( chain_r(tmp1)(tmp2)(tmp3) ) );          \
}                                                                              \
/**/

#endif

