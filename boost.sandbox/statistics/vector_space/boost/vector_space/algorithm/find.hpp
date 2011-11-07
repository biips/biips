///////////////////////////////////////////////////////////////////////////////
// vector_space::algorithm::find.hpp                                         //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_VECTOR_SPACE_FIND_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_VECTOR_SPACE_FIND_HPP_ER_2009
#include <numeric>
#include <cmath>
#include <boost/vector_space/functional/equal.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace vector_space{
    
    // This is a version of find where the elements in *[b,e) are ranges.
    // The value eps is a minimum proximity distance that if not met
    // causes the algorithm to return e.
    template<template<typename> class F,typename In,typename X>
    In find(
        In b, // *In is a range
        In e,
        const X& x,
        const typename range_value<X>::type& eps
    ){
        return std::find_if(
            b,
            e,
            functional::equal(x,eps)
        );
    }

    template<typename In,typename X>
    In find(
        In b, 
        In e,
        const X& x
    ){
        return std::find_if(
            b,
            e,
            functional::equal(x)
        );
    }
    
}// vector_space
}// detail
}// statistics
}// boost

#endif