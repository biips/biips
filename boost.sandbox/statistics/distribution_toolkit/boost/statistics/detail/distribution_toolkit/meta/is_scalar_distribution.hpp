///////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::meta::is_distribution.hpp                           //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_META_IS_SCALAR_DISTRIBUTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_META_IS_SCALAR_DISTRIBUTION_HPP_ER_2009
#include <boost/statistics/detail/distribution_toolkit/meta/is_math_distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{
namespace meta{

    template<typename D>
    struct is_scalar_distribution : is_math_distribution<D> {};

    // Specialize to bool_<true> for those distribution defined within
    // distribution_toolkit, such as location_scale


}// meta
}// distribution
}// toolkit
}// detail
}// statistics
}// boost

#endif

