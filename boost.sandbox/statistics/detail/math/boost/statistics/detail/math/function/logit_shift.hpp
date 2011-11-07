///////////////////////////////////////////////////////////////////////////////
// math::logit_shift.hpp                                                     //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_MATH_FUNCTION_LOGIT_SHIFT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_MATH_FUNCTION_LOGIT_SHIFT_HPP_ER_2009
#include <boost/statistics/detail/math/function/log_shift.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace math{

    // The t>0 is an adjustment which preventsa numerical error at p = 0 and 
    // p = 1 causes numerical error
    template<typename T>
    T logit_shift(const T& p,const T& t){
        // log(p/(1-p)) = log(p)-log(1-p)
        return log_shift(p,t) - boost::math::log1p(t - p);
    }

}// math
}// detail
}// statistics
}// boost

#endif