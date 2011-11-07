//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_fifo.hpp                                //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_FIFO_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_FIFO_ER_2010_HPP
#include <set>
#include <stdexcept>

#ifndef BOOST_ASSIGN_CHECK_EQUAL
#error
#endif

#define BOOST_ASSIGN_AS_CHECK_adapter_fifo(C)                                  \
{                                                                              \
    typedef T val_;                                                            \
    using namespace check_constants;                                           \
                                                                               \
    C elems = BOOST_ASSIGN_AS_CHECK_cref8(a,b,c,d,e,f,g,h).to_adapter();       \
    BOOST_ASSIGN_CHECK_EQUAL(elems.front() , a); elems.pop();                  \
    BOOST_ASSIGN_CHECK_EQUAL(elems.front() , b); elems.pop();                  \
    BOOST_ASSIGN_CHECK_EQUAL(elems.front() , c); elems.pop();                  \
    BOOST_ASSIGN_CHECK_EQUAL(elems.front() , d); elems.pop();                  \
    BOOST_ASSIGN_CHECK_EQUAL(elems.front() , e); elems.pop();                  \
    BOOST_ASSIGN_CHECK_EQUAL(elems.front() , f); elems.pop();                  \
    BOOST_ASSIGN_CHECK_EQUAL(elems.front() , g); elems.pop();                  \
    BOOST_ASSIGN_CHECK_EQUAL(elems.front() , h); elems.pop();                  \
    BOOST_ASSIGN_CHECK_EQUAL(elems.empty() , true);                            \
}                                                                              \
/**/

#endif
