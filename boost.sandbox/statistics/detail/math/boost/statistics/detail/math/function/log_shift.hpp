///////////////////////////////////////////////////////////////////////////////
// math::log_shift.hpp                                                       //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_MATH_FUNCTION_LOG_SHIFT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_MATH_FUNCTION_LOG_SHIFT_HPP_ER_2009

namespace boost{
namespace statistics{
namespace detail{
namespace math{

    template<typename T>
    T log_shift(const T& x,const T& t){
        return log(x+t);
    }

}// math
}// detail
}// statistics
}// boost

#endif