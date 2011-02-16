//////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::gamma::log_unnormalized_pdf         //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_GAMMA_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_GAMMA_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#include <cmath>
#include <boost/math/distributions/gamma.hpp>
#include <boost/math/policies/policy.hpp>

namespace boost{
namespace math{

template <class T, class P>
inline T log_unnormalized_pdf(
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
      // TODO just a guess based on definition of math::pdf
      // cast is needed by MSVC
      return log(static_cast<T>(0));
   }
   static T one_ = static_cast<T>(1);
   result = (shape - one_) * log(x) - x / scale;
   return result;
} // log_unnormalized_pdf

}// math
}// boost

#endif
