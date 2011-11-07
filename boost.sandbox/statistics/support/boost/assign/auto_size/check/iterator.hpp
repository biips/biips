//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_iterator.hpp                            //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_ITERATOR_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_ITERATOR_ER_2010_HPP
#include <boost/range.hpp>
#include <boost/assign/auto_size/check/constants.hpp>

#define BOOST_ASSIGN_AS_CHECK_iterator                                         \
{                                                                              \
    using namespace check_constants;                                           \
    check_iterator_impl(                                                       \
        boost::begin( BOOST_ASSIGN_AS_CHECK_cref8(a,b,c,d,e,f,g,h)) );         \
}                                                                              \
/**/

#endif
