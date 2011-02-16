//////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::gamma::is_log_concave               //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_GAMMA_IS_LOG_CONCAVE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_GAMMA_IS_LOG_CONCAVE_HPP_ER_2009
#include <cmath>
#include <boost/math/distributions/gamma.hpp>
#include <boost/math/policies/policy.hpp>

namespace boost{
namespace math{

template <class T, class P>
inline bool is_log_concave(
    const boost::math::gamma_distribution<T, P>& dist
){
    return ( dist.shape() > static_cast<T>(1) );
}

}// math
}// boost

#endif
