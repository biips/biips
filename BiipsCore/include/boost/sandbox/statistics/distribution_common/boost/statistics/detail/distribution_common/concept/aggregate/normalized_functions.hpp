//////////////////////////////////////////////////////////////////////////////////
// distribution::common::concept::aggregate::normalized_functions.hpp           //
//                                                                              //
//  (C) Copyright 2009 Erwann Rogard                                            //
//  Use, modification and distribution are subject to the                       //
//  Boost Software License, Version 1.0. (See accompanying file                 //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)            //
//////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_CONCEPT_AGGREGATE_NORMALIZED_FUNCTIONS_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_CONCEPT_AGGREGATE_NORMALIZED_FUNCTIONS_HPP_ER_2009
#include <boost/mpl/empty_base.hpp>
#include <boost/statistics/detail/distribution_common/concept/unary/cdf.hpp>
#include <boost/statistics/detail/distribution_common/concept/unary/pdf.hpp>
#include <boost/statistics/detail/distribution_common/concept/unary/quantile.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace common{
namespace concept{

    template<
        typename B
    >
    class NormalizedFunctions : public
        Cdf<
            Pdf<
                Quantile<
                    B
                >
            >
        >
    {
    };

}// concept
}// common
}// distribution

}// statistics
}// detail
}// boost

#endif

