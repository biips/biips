//////////////////////////////////////////////////////////////////////////////
// distribution::common::reference::wrapper::log_unnormalized_pdf.hpp       //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_DISTRIBUTIONS_REFERENCE_WRAPPER_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_DISTRIBUTIONS_REFERENCE_WRAPPER_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#include <boost/mpl/empty_base.hpp>
#include <boost/call_traits.hpp>
#include <boost/statistics/detail/distribution_common/meta/value.hpp>
#include <boost/statistics/detail/distribution_common/distributions/reference/wrapper.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{

    template<typename D,typename X>
    typename reference_wrapper<D>::value_type
    log_unnormalized_pdf(const reference_wrapper<D>& rw, const X& x)
    {
        return log_unnormalized_pdf(rw.distribution(),x);
    }

}// distribution    
}// detail
}// statistics
}// boost

#endif
