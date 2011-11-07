///////////////////////////////////////////////////////////////////////////////
// math::logit_shift.hpp                                                     //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_MATH_FUNCTION_LOGIT_SHIFT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_MATH_FUNCTION_LOGIT_SHIFT_HPP_ER_2009
#include <boost/statistics/detail/math/function/logit_shift.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace math{

    template<typename T>
    T logit(
        const T& p
    ){
        return logit_shift(p,static_cast<T>(0));
    }

}
}
}
}

#endif