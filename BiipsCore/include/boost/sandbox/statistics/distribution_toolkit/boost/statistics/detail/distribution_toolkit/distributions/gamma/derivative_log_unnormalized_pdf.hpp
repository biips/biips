//////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::gamma::derivative::log_unnormalized_pdf //
//                                                                              //
//  (C) Copyright 2009 Erwann Rogard                                            //
//  Use, modification and distribution are subject to the                       //
//  Boost Software License, Version 1.0. (See accompanying file                 //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)            //
//////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_GAMMA_DERIVATIVE_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_GAMMA_DERIVATIVE_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#include <cmath>
#include <limits>
#include <boost/math/distributions/gamma.hpp>
#include <boost/math/policies/policy.hpp>

namespace boost{
namespace math{

template <class T, class P>
inline T derivative_log_unnormalized_pdf(
    const boost::math::gamma_distribution<T, P>& dist,
    const T& x
)
{
   BOOST_MATH_STD_USING  // for ADL of std functions

   static const char* function
        = "log_unnormalized_pdf(const gamma_distribution<%1%>&, %1%)";

   T shape = dist.shape();
   T scale = dist.scale();

   T result;
   if(false == boost::math::detail::check_gamma(
    function, scale, shape, &result, P()))
      return result;
   if(false == boost::math::detail::check_gamma_x(function, x, &result, P()))
      return result;

   if(x == 0)
   {
      // TODO check. Also see log_unnormalized_pdf
      return std::numeric_limits<T>::infinity();
   }
   static T one_ = static_cast<T>(1);
   result = (shape - one_) / x - one_ / scale;
   return result;
} // derivative_log_unnormalized_pdf


}// math
}// boost

#endif
