///////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::normal::normalizing_constant.hpp         //
//                                                                               //
//  (C) Copyright 2009 Erwann Rogard                                             //
//  Use, modification and distribution are subject to the                        //
//  Boost Software License, Version 1.0. (See accompanying file                  //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_NORMAL_NORMALIZING_CONSTANT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_NORMAL_NORMALIZING_CONSTANT_HPP_ER_2009
#include <cmath>
#include <boost/math/constants/constants.hpp>
#include <boost/math/distributions/normal.hpp>

namespace boost{
namespace math{

template<typename T,typename P>
T normalizing_constant(const boost::math::normal_distribution<T,P>& d){
    static T pi = boost::math::constants::pi<T>;
    static T two = static_cast<T>(2);
    return sqrt(two * pi) * d.scale();
}

}// math
}// boost

#endif
