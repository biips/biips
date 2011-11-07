//////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::students_t::log_unnormalized_pdf.hpp    //
//                                                                              //
//  (C) Copyright 2009 Erwann Rogard                                            //
//  Use, modification and distribution are subject to the                       //
//  Boost Software License, Version 1.0. (See accompanying file                 //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)            //
//////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_STUDENTS_T_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_STUDENTS_T_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#include <boost/math/distributions/students_t.hpp>
#include <boost/math/special_functions/log1p.hpp>
#include <boost/numeric/conversion/converter.hpp>
// #include <boost/math/policies/policy.hpp> // TODO

namespace boost{
namespace math{

    template<typename T,typename Policy>
    T
    log_unnormalized_pdf(
        const boost::math::students_t_distribution<T,Policy>& d,
        const T& x
    ){

        typedef boost::numeric::converter<T,int> int2R_t;

        T r1 = int2R_t::convert(1);
        T r2 = int2R_t::convert(2);

        T nu = d.degrees_of_freedom();
        T m = ( nu + r1 ) / r2;
        T y = ( x * x ) / nu;
        return (- m ) * math::log1p(y);
    }

}// math
}// boost

#endif
