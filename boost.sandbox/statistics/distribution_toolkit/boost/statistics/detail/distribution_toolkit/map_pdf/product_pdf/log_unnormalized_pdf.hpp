//////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::map_pdf::product_pdf::log_unnomralized_pdf.hpp     //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_PRODUCT_PDF_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_PRODUCT_PDF_LOG_UNNORMALIZED_PDF_HPP_ER_2009
#include <boost/mpl/not.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/statistics/detail/distribution_toolkit/map_pdf/product_pdf/product_pdf.hpp>
#include <boost/statistics/detail/distribution_common/meta/value.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

    template<typename A,typename B,typename T>
    T log_unnormalized_pdf(const product_pdf<A,B>& dist,const T& x)
    {
        return
            (log_unnormalized_pdf(dist.first(),x)
                + log_unnormalized_pdf(dist.second(),x));
    }

}// toolkit
}// distribution
}// detail
}// statistics
}// boost


#endif 
