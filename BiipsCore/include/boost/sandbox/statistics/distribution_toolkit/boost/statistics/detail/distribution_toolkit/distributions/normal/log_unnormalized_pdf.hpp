//////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::::normal::log_unnormalized_pdf.hpp  //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_NORMAL_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_NORMAL_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#include <boost/math/distributions/normal.hpp>
#include <boost/numeric/conversion/converter.hpp>
//#include <boost/math/policies/policy.hpp>//TODO

namespace boost{
namespace math{

    template<typename T,typename P>
    T
    log_unnormalized_pdf(
        const boost::math::normal_distribution<T,P>& d,
        const T& x
    ){
        typedef boost::numeric::converter<T,int> int2real_t;
        T mu = d.location();
        T sigma = d.scale();

        T z = (x-mu)/sigma;
        return - z * z / int2real_t::convert(2);
    }
    
}// math
}// boost

#endif
