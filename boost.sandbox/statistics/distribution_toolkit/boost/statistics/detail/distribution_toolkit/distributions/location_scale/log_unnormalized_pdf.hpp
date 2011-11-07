///////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::location_scale::log_unnormalized_pdf.hpp //
//                                                                               //
//  (C) Copyright 2009 Erwann Rogard                                             //
//  Use, modification and distribution are subject to the                        //
//  Boost Software License, Version 1.0. (See accompanying file                  //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#include <boost/concept/assert.hpp>
#include <boost/statistics/detail/distribution_toolkit/distributions/location_scale/location_scale.hpp>
#include <boost/statistics/detail/distribution_common/meta/value.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

    template<typename Z,typename T>
    T
    log_unnormalized_pdf(
        const distribution::toolkit::location_scale_distribution<Z>& d,
        const T& x
    ){
        typedef distribution::toolkit::location_scale_distribution<Z> dist_;

        T z = (x-d.mu())/d.sigma();
        T result = log_unnormalized_pdf(d.z(),z); 
        // -log ( sigma ) is a constant so it is ignored
        return result;
    }

}// toolkit
}// distribution
}// detail
}// statistics 
}// boost

#endif
