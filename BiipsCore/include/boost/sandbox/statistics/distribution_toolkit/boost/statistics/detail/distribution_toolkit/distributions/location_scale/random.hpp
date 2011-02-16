/////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::location_scale::random.hpp             //
//                                                                             //
//  (C) Copyright 2009 Erwann Rogard                                           //
//  Use, modification and distribution are subject to the                      //
//  Boost Software License, Version 1.0. (See accompanying file                //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)           //
/////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_RANDOM_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_RANDOM_HPP_ER_2009
#include <boost/statistics/detail/distribution_toolkit/distributions/location_scale/location_scale.hpp>
#include <boost/random/location_scale.hpp>
#include <boost/statistics/detail/distribution_common/meta/random/distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace meta{

    template<typename Z>
    struct random_distribution< 
        toolkit::location_scale_distribution<Z> 
    >{
        typedef toolkit::location_scale_distribution<Z> dist_;
        typedef typename meta::random_distribution<Z>::type z_;
        typedef boost::random::location_scale_distribution<z_> type;
        
        static type call(const dist_& d){ 
            return type(
                d.mu(),
                d.sigma(),
                make_random_distribution(d.z())
            ); 
        }
    };

    
}// meta
}// distribution
}// detail
}// statistics
}// boost

#endif
