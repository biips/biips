///////////////////////////////////////////////////////////////////////////////
// numeric::algorithm::min_element.hpp                                       //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NUMERIC_ALGORITHM_MIN_ELEMENT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_NUMERIC_ALGORITHM_MIN_ELEMENT_HPP_ER_2009
#include <limits>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/limits.hpp>
#include <boost/concept_check.hpp>
#include <boost/concept/assert.hpp>
#include <iostream>

namespace boost{
namespace statistics{
namespace detail{
namespace numeric{
namespace algorithm{

// This is a rewrite of std::min_element with one additional argument, m, which
// is assigned the min value of *i over [first,last).
//
// Rationale: saves one dereferencing operation,
// which may be worthwhile if dereferencing is expensive. 
template <typename It,typename V>
It min_element(It first, It last,V& min_value) {
    typedef typename iterator_value<It>::type value_type;

    // TODO see about enable_if/disable_if
    static bool has_inf = std::numeric_limits<V>::has_infinity;
    static V inf_if = std::numeric_limits<V>::infinity(); 
    static V highest = boost::numeric::bounds<V>::highest();
    static V inf_ = has_inf? inf_if : highest;
    
    BOOST_CONCEPT_ASSERT((
        boost::IncrementableIterator<It>
    ));
    
    min_value = inf_;
    value_type value;
    It result = first;
    while (first != last){
        value = (*first);
        if (value < min_value){
            min_value = value;
            result = first;
        }

        ++first;
    }
    return result;
}

}// algorithm
}// numeric
}// detail
}// statistics
}// boost

#endif