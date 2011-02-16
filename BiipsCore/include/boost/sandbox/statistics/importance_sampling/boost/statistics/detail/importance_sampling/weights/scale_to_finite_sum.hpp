///////////////////////////////////////////////////////////////////////////////
// importance_sampling::scale_to_finite_sum.hpp                              //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_WEIGHTS_SCALE_TO_FINITE_SUM_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_WEIGHTS_SCALE_TO_FINITE_SUM_HPP_ER_2009
#include <numeric>
#include <boost/lambda/lambda.hpp>
#include <boost/range.hpp>
#include <boost/statistics/detail/importance_sampling/weights/find_scale_to_finite_sum.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace importance_sampling{

    // Scales each element [b,e) by the smallest factor, c, such that the sum 
    // is finite.
    //
    // [ Warning ] c is not insensitive to permutations of [b,e), due
    // to non-associativity in the fp system
    template<typename InIt>
    typename iterator_value<InIt>::type 
    scale_to_finite_sum(InIt b,InIt e,
        typename iterator_value<InIt>::type low,
        typename iterator_value<InIt>::type high
    );

    template<typename InIt>
    typename iterator_value<InIt>::type 
    scale_to_finite_sum(InIt b,InIt e);

    // Implementation //

    template<typename InIt>
    typename iterator_value<InIt>::type 
    scale_to_finite_sum(InIt b,InIt e,
        typename iterator_value<InIt>::type low,
        typename iterator_value<InIt>::type high
    ){
        typedef typename iterator_value<InIt>::type val_;
        val_ c = find_scale_finite_sum(
            b,e,low,high
        );
        std::transform(
            b,
            e,
            b,
            boost::lambda::_1 / c
        );
        return c;
    }

    template<typename InIt>
    typename iterator_value<InIt>::type 
    scale_to_finite_sum(InIt b,InIt e){
        typedef typename iterator_value<InIt>::type val_;
        val_ c = find_scale_to_finite_sum(b,e);
        std::transform(
            b,
            e,
            b,
            boost::lambda::_1 / c
        );
        return c;
    }

}// importance_weights
}// detail
}// statistics
}// boost

#endif