/////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::normal::random.hpp                     //
//                                                                             //
//  (C) Copyright 2009 Erwann Rogard                                           //
//  Use, modification and distribution are subject to the                      //
//  Boost Software License, Version 1.0. (See accompanying file                //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)           //
/////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_NORMAL_RANDOM_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_NORMAL_RANDOM_HPP_ER_2009
#include <boost/math/distributions/normal.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/statistics/detail/distribution_common/meta/random/distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace meta{

    template<typename T,typename P>
    struct random_distribution< 
        boost::math::normal_distribution<T,P> 
    >{
        typedef boost::math::normal_distribution<T,P> dist_;
        typedef boost::normal_distribution<T> type;
        
        static type call(const dist_& d){ 
            return type(d.location(),d.scale()); 
        }
    };
    
}// meta
}// distribution
}// detail
}// statistics
}// boost

#endif
