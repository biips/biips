//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_converter.hpp                           //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_CONVERTER_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_CONVERTER_ER_2010_HPP
#include <boost/array.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/assign/auto_size/check/constants.hpp>

#ifndef BOOST_ASSIGN_CHECK_EQUAL
#error
#endif

#define BOOST_ASSIGN_AS_CHECK_converter(C)                                     \
{                                                                              \
    typedef T val_;                                                            \
                                                                               \
    using namespace boost::assign;                                             \
    using namespace check_constants;                                           \
    C elems = BOOST_ASSIGN_AS_CHECK_cref8(a,b,c,d,e,f,g,h);                    \
    check_iterator_impl(boost::begin(elems));                                  \
}                                                                              \
/**/

#define BOOST_ASSIGN_AS_CHECK_converter_sorted(C)                              \
{                                                                              \
    typedef T val_;                                                            \
                                                                               \
    using namespace boost::assign;                                             \
    using namespace check_constants;                                           \
    C elems = BOOST_ASSIGN_AS_CHECK_cref8(a,b,c,d,e,f,g,h);                    \
    check_iterator_sorted_impl(boost::begin(elems));                           \
}                                                                              \
/**/

#define BOOST_ASSIGN_AS_CHECK_converter_array                                  \
{                                                                              \
    typedef boost::array<T,8> ar8_;                                            \
    BOOST_ASSIGN_AS_CHECK_converter(ar8_)                                      \
}                                                                              \
/**/

#endif
