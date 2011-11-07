//////////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::location_scale::description.hpp             //
//                                                                                  //
//  (C) Copyright 2009 Erwann Rogard                                                //
//  Use, modification and distribution are subject to the                           //
//  Boost Software License, Version 1.0. (See accompanying file                     //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)                //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_DESCRIPTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_DESCRIPTION_HPP_ER_2009
#include <string>
#include <boost/format.hpp>
#include <boost/statistics/detail/distribution_toolkit/distributions/location_scale/location_scale.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{


    template<typename Z>
    std::string
    description(
        const location_scale_distribution<Z>& d
    )
    {
        const char* msg = "location_scale_distribution( %1% + %2% %3%)";
        format f(msg); f % d.mu() % d.sigma() % description(d.z());
        return f.str();
    }

}// distribution
}// toolkit

}// detail
}// statistics
}// boost

#endif
