///////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::map_pdf::product_pdf::is_pseudo_scalar_distribution.hpp //
//                                                                           //
//  (C) Copyright 2009 Erwann Rogard                                         //
//  Use, modification and distribution are subject to the                    //
//  Boost Software License, Version 1.0. (See accompanying file              //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_PRODUCT_PDF_IS_PSEUDO_SCALAR_DISTRIBUTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_PRODUCT_PDF_IS_PSEUDO_SCALAR_DISTRIBUTION_HPP_ER_2009
#include <boost/mpl/bool.hpp>
#include <boost/mpl/and.hpp>
#include <boost/statistics/detail/distribution_toolkit/meta/is_pseudo_scalar_distribution.hpp>
#include <boost/statistics/detail/distribution_toolkit/map_pdf/product_pdf/product_pdf.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

namespace meta{

    template<typename A,typename B>
    struct is_pseudo_scalar_distribution< 
        product_pdf<A,B> 
    > : mpl::and_<
        is_pseudo_scalar_distribution<A>,
        is_pseudo_scalar_distribution<B>
    > {};
    
}// meta
}// distribution
}// toolkit

}// detail
}// statistics
}// boost

#endif
