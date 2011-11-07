//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_copy_iterator.hpp                       //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_COPY_ITERATOR_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_COPY_ITERATOR_ER_2010_HPP
#include <boost/typeof/typeof.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/assign/auto_size/check/constants.hpp>

#define BOOST_ASSIGN_AS_CHECK_copy_iterator                                    \
{                                                                              \
    typedef T val_;                                                            \
    using namespace check_constants;                                           \
    val_ a1, b1, c1, d1, e1, f1, g1, h1;                                       \
    BOOST_AUTO(tmp,BOOST_ASSIGN_AS_CHECK_ref8(a1,b1,c1,d1,e1,f1,g1,h1));       \
    boost::copy(array(), boost::begin(tmp));                                   \
    check_values_impl(a1,b1,c1,d1,e1,f1,g1,h1);                                \
}                                                                              \
/**/

#endif
