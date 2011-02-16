///////////////////////////////////////////////////////////////////////////////
// importance_sampling::apply_exp_offset                                     //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_WEIGHTS_APPLY_EXP_OFFSET_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_WEIGHTS_APPLY_EXP_OFFSET_HPP_ER_2009
#include <cmath>
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/math/tools/precision.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace importance_sampling{
    
    // Returns          The smallest value, c, s/t *i + c <= t, i in [b_w,e_w)
    // Side effect:     *i <- exp(*i+c)
    //
    // The greater c, the higher the precision but also risk of isinf.
    // Textbook often show t = 0 so that exp(w+c) <= 1. 
    template<typename It>
    typename iterator_value<It>::type
    apply_exp_offset(
        It b,
        It e,
        typename iterator_value<It>::type t
    ){
        typedef typename iterator_value<It>::type val_;
        val_ max = *std::max_element(b,e);
        val_ offset = (t - max); 
        typedef val_(*fp_)(val_);
        fp_ fp = std::exp;
        std::transform(
        	b,
            e,
            b,
            lambda::bind<val_>(fp,lambda::_1 + offset)
        );
        return offset;
    }

    // Same as above, but t set such that exp(t+epsilon) = inf, exp(t)<inf 
    template<typename It>
    typename iterator_value<It>::type
    apply_exp_offset(
        It b,
        It e
    ){
        typedef typename iterator_value<It>::type val_;
        const val_ log_max = boost::math::tools::log_max_value<val_>();

        return apply_exp_offset(
            b,
            e,
            log_max
        );
    }

}// importance_weights
}// detail
}// statistics
}// boost

#endif