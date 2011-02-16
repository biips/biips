///////////////////////////////////////////////////////////////////////////////
// importance_sampling::maximal_finite_sums.hpp                              //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_WEIGHTS_MAXIMAL_FINITE_SUMS_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_WEIGHTS_MAXIMAL_FINITE_SUMS_HPP_ER_2009
#include <boost/iterator/iterator_traits.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace importance_sampling{

    // Breaks down the summation of *i, i in [b,e) into maximal finite amounts,
    // each of which are copied to the output iterator i
    //
    // Note: Primarily for use by find_scale_to_finite_sum
    template<typename InIt,typename OutIt>
    OutIt maximal_finite_sums(InIt b, InIt e, OutIt i)
    {
        typedef typename iterator_value<InIt>::type value_type;
        value_type sum = static_cast<value_type>(0);
        while(b!=e){
            value_type d = *b;
            if( boost::math::isinf( sum + d ) ){
                *i = sum;
                sum = d;
                ++i;
            }else{
                sum += d;
            }
            ++b;
        }
        return i;
    };
    
}// importance_weights
}// detail
}// statistics
}// boost

#endif