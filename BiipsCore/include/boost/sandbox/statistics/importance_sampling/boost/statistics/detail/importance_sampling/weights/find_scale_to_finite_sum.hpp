///////////////////////////////////////////////////////////////////////////////
// importance_sampling::find_scale_to_finite_sum.hpp                         //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_WEIGHTS_FIND_SCALE_TO_FINITE_SUM_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_WEIGHTS_FIND_SCALE_TO_FINITE_SUM_HPP_ER_2009
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <boost/lambda/lambda.hpp>
#include <boost/range.hpp>
#include <boost/format.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/math/tools/precision.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/statistics/detail/importance_sampling/weights/maximal_finite_sums.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace importance_sampling{

    // Finds c such that the sum over sum{*i/c, i in [b,e)} < inf using the
    // bisection method.
    //
    // Warning: The value c is not insensitive to permutations of [b,e), due
    // to non-associativity in the fp system
    template<typename InIt>
    typename iterator_value<InIt>::type 
    find_scale_to_finite_sum(
        InIt b,InIt e,
        typename iterator_value<InIt>::type low_init,
        typename iterator_value<InIt>::type high_init
    );
    
    // This version may be faster than that above with low = 1, high = highest
    template<typename InIt>
    typename iterator_value<InIt>::type 
    find_scale_to_finite_sum(InIt b,InIt e);

    // Implementation //

    template<typename InIt>
    typename iterator_value<InIt>::type 
    find_scale_to_finite_sum(
        InIt b,InIt e,
        typename iterator_value<InIt>::type low_init,
        typename iterator_value<InIt>::type high_init
    ){
        typedef typename iterator_value<InIt>::type val_;
        static val_ zero = static_cast<val_>(0);
        static val_ two = static_cast<val_>(2);
        static val_ eps = boost::math::tools::epsilon<val_>();

        BOOST_ASSERT(low_init < high_init);
        BOOST_ASSERT(low_init > zero);
        BOOST_ASSERT(high_init > zero);
        val_ low = low_init;
        val_ high = high_init;
        val_ mid = (low + high)/two;
        val_ delta, acc;
        
        do{
            delta = high - low;
            acc =
                std::accumulate(b,e,zero, lambda::_1 + ( lambda::_2 / mid) );
            if(boost::math::isinf(acc)){
                low = mid;
            }else{
                high = mid;
            }
            mid = (low+high)/two;
        }while(
            delta - (high - low)>eps
        );
        static  
            const char* str = "%3% = find_scale_to_finite_sum(b,e,%1%,%2%) = inf";
        if(
            boost::math::isinf(
                std::accumulate(b,e,zero, lambda::_1 + ( lambda::_2 / high) )
            )
        ){
            format f(str); f%low_init%high_init%high;
            throw std::runtime_error(f.str());
        }
        return high;
    }

    template<typename InIt>
    typename iterator_value<InIt>::type 
    find_scale_to_finite_sum(InIt b,InIt e){
        typedef typename iterator_value<InIt>::type val_;
        typedef numeric::bounds<val_>               bounds_;
        typedef std::vector<val_>                   vec_;
        static val_ zero = static_cast<val_>(0);
        static val_ one = static_cast<val_>(1);
        static val_ two = static_cast<val_>(2);
        static val_ highest = bounds_::highest();
        vec_ vec;
        maximal_finite_sums(b,e,std::back_inserter(vec));
        val_ low = one;
        val_ high = highest;
        val_ mid = find_scale_to_finite_sum(
            boost::begin(vec),
            boost::end(vec),
            low,high
        );
        low = mid;
        high = mid;
        while( 
            !boost::math::isinf(
                std::accumulate(b,e,zero, lambda::_1 + ( lambda::_2 / low) )
            )
        ){
            low /= two;
            if(low<one){
                low = one;
                break;
            }
        }
        while( 
            boost::math::isinf(
                std::accumulate(b,e,zero, lambda::_1 + ( lambda::_2 / high) )
            )
        ){
            high *= two;
            if(boost::math::isinf(high)){
                high = highest;
                break;
            }
        }
        return find_scale_to_finite_sum(b,e,low,high);
    }

}// importance_weights
}// detail
}// statistics
}// boost

#endif