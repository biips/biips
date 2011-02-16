///////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::meta::is_pseudo_scalar_distribution.hpp             //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_META_IS_PSEUDO_SCALAR_DISTRIBUTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_META_IS_PSEUDO_SCALAR_DISTRIBUTION_HPP_ER_2009
#include <boost/statistics/detail/distribution_toolkit/meta/is_scalar_distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

namespace meta{

    // The mapping of a distribution's pdf is not a distribution, but for 
    // practical purposes, it may share its interface such as pdf(dist,x)
    template<typename D> 
    struct is_pseudo_scalar_distribution 
        : is_scalar_distribution<D> {};

}// meta
}// distribution
}// toolkit

}// detail
}// statistics
}// boost

#endif
