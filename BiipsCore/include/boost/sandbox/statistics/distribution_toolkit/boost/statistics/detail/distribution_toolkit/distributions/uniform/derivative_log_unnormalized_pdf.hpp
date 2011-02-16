//////////////////////////////////////////////////////////////////////////////
// distributions::unnormalized_pdf::uniform.hpp                             //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_UNIFORM_DERIVATIVE_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_UNIFORM_DERIVATIVE_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#include <boost/math/distributions/uniform.hpp>
#include <boost/math/special_functions/log1p.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/math/policies/policy.hpp>
#include <boost/math/tools/precision.hpp>

namespace boost{
namespace math{

    template<typename T,typename P>
    T
    derivative_log_unnormalized_pdf(
        const boost::math::uniform_distribution<T,P>& dist,
        const T& x
    ){
        T lower = dist.lower();
        T upper = dist.upper();
        T result; // of checks.
        if(false == boost::math::detail::check_uniform(
            "boost::math::pdf(const uniform_distribution<%1%>&, %1%)", 
            lower, upper, &result, P()))
        {
            return result;
        }
        if(false == math::detail::check_uniform_x(
            "boost::math::pdf(const uniform_distribution<%1%>&, %1%)",
             x, &result, P()))
        {
            return result;
        }

        if((x < lower) || (x > upper) )
        {
            // TODO. Makes sense?
            static T inf = std::numeric_limits<T>::infinity();
            return (- inf);
        }
        else
        {
            return static_cast<T>(0);
        }
    }

}// math
}// boost

#endif
