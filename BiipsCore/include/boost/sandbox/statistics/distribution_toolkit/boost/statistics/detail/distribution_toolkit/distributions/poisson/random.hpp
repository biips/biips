/////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::poisson::random.hpp               	   //
//                                                                             //
//  (C) Copyright 2010 Erwann Rogard                                           //
//  Use, modification and distribution are subject to the                      //
//  Boost Software License, Version 1.0. (See accompanying file                //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)           //
/////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_POISSON_RANDOM_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_POISSON_RANDOM_HPP_ER_2010
#include <boost/math/distributions/poisson.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/statistics/detail/distribution_common/meta/random/distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace meta{

    template<typename T,typename P>
    struct random_distribution< 
        boost::math::poisson_distribution<T,P> 
    >{
        typedef boost::math::poisson_distribution<T,P> dist_;
        typedef boost::poisson_distribution<int,T> type;
        
        static type call(const dist_& d){ 
            return type(d.mean()); 
        }
    };
    
}// meta
}// distribution
}// detail
}// statistics
}// boost

#endif