/////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::chi_squared::log_unnormalized_pdf.hpp  //
//                                                                             //
//  (C) Copyright 2009 Erwann Rogard                                           //
//  Use, modification and distribution are subject to the                      //
//  Boost Software License, Version 1.0. (See accompanying file                //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)           //
/////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_CHI_SQUARED_LOG_UNNORMALIZED_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_CHI_SQUARED_LOG_UNNORMALIZED_HPP_ER_2009
#include <boost/math/distributions/chi_squared.hpp>
#include <boost/math/special_functions/log1p.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/math/policies/policy.hpp>

namespace boost{
namespace math{

    template<typename T,typename P>
    T log_unnormalized_pdf(
        const boost::math::chi_squared_distribution<T,P>& dist,
        const T& x
    ){
        BOOST_MATH_STD_USING  // for ADL of std functions
        T degrees_of_freedom = dist.degrees_of_freedom();
        // Error check:
        T error_result;

        static const char* function = 
        "log_unnormalized_pdf(const chi_squared_distribution<%1%>&, %1%)";

        if(false == boost::math::detail::check_df(
            function, degrees_of_freedom, &error_result, P()))
                return error_result;

        if((x <= 0) || !(boost::math::isfinite)(x))
        {
            return boost::math::policies::raise_domain_error<T>(
            function, "Chi Square parameter was %1%, but must be > 0 !", 
            x, P());
        }

        // Lumped case x == 0 in x<=0 above

        static T one = static_cast<T>(1);
        static T two = static_cast<T>(2);
        return (degrees_of_freedom/two-one) * log(x) - x/two;
    }

}// math
}// boost

#endif
