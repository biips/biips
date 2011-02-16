///////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::meta::is_math_distribution.hpp                      //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_META_IS_MATH_DISTRIBUTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_META_IS_MATH_DISTRIBUTION_HPP_ER_2009
#include <boost/mpl/bool.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

namespace meta{

    template<typename D>
    struct is_math_distribution : boost::mpl::bool_<false> {};

    // Specialize to bool_<true> for distribution defined in boost::math only


}// meta
}// distribution
}// toolkit

}// detail
}// statistics
}// boost

#endif
