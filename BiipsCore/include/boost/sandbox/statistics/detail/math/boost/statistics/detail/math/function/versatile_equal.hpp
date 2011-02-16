///////////////////////////////////////////////////////////////////////////////
// math::function::versatile_equal.hpp                                       //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_MATH_FUNCTION_VERSATILE_EQUAL_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_MATH_FUNCTION_VERSATILE_EQUAL_HPP_ER_2009
#include <cmath>
#include <boost/type_traits/is_float.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/math/tools/precision.hpp>

namespace boost{
namespace statistics{ 
namespace detail{
namespace math{
namespace function{

    template<typename T>
    typename boost::enable_if<boost::is_float<T>,bool>::type
    versatile_equal(const T& t1,const T& t2)
    {
        static const T eps = boost::math::tools::epsilon<T>();
        return std::fabs(t1-t2)<eps;
    } // warning: consider relative difference

    template<typename T>
    typename boost::enable_if<boost::is_integral<T>,bool>::type
    versatile_equal(const T& t1,const T& t2){
        return (t1 == t2);
    }


}// function
}// math
}// detail
}// statistics
}// boost

#endif