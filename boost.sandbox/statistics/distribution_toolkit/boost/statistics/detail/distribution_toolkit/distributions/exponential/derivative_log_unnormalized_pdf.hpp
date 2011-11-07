///////////////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::exponential::derivative_log_unnormalized_pdf.hpp //
//                                                                                       //
//  (C) Copyright 2009 Erwann Rogard                                                     //
//  Use, modification and distribution are subject to the                                //
//  Boost Software License, Version 1.0. (See accompanying file                          //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)                     //
///////////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_EXPONENTIAL_DERIVATIVE_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_EXPONENTIAL_DERIVATIVE_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#include <string>
#include <boost/math/distributions/exponential.hpp>
#include <boost/math/policies/policy.hpp> 

namespace boost{
namespace math{

    template<typename T,typename P>
    T
    derivative_log_unnormalized_pdf(
        const boost::math::exponential_distribution<T,P>& d,
        const T& x
    ){
        typedef std::string str_;
        BOOST_MATH_STD_USING // for ADL of std functions
        // to keep width = 80 caracters
        static const char* function = (
            str_("derivative_log_unnormalized_pdf(")
            +"const exponential_distribution<%1%>&,%1%)"
        ).c_str();

        T lambda = d.lambda();
        T result;
        if(0 == boost::math::detail::verify_lambda(
            function, lambda, &result, P()))
            return result;
        if(0 == boost::math::detail::verify_exp_x(
            function, x, &result, P()))
            return result;
        result = (-lambda);
        return result;
    }

}// math
}// boost

#endif
