//////////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::location_scale::is_scalar_distribution.hpp  //
//                                                                                  //
//  (C) Copyright 2009 Erwann Rogard                                                //
//  Use, modification and distribution are subject to the                           //
//  Boost Software License, Version 1.0. (See accompanying file                     //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)                //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_IS_SCALAR_DISTRIBUTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_IS_SCALAR_DISTRIBUTION_HPP_ER_2009
#include <boost/mpl/bool.hpp>
#include <boost/statistics/detail/distribution_toolkit/distributions/location_scale/location_scale.hpp>
#include <boost/statistics/detail/distribution_toolkit/meta/is_scalar_distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

namespace meta{

    template<typename Z>
    struct is_scalar_distribution< location_scale_distribution<Z> > 
        : is_scalar_distribution<Z>{};

}// meta
}// distribution
}// toolkit

}// detail
}// statistics
}// boost

#endif
