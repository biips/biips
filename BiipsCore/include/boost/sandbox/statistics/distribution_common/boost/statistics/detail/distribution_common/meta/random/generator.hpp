//////////////////////////////////////////////////////////////////////////////////
// distribution::common::meta::random::generator.hpp                            //
//                                                                              //
//  (C) Copyright 2009 Erwann Rogard                                            //
//  Use, modification and distribution are subject to the                       //
//  Boost Software License, Version 1.0. (See accompanying file                 //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)            //
//////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_RANDOM_GENERATOR_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_RANDOM_GENERATOR_HPP_ER_2009
#include <boost/random/variate_generator.hpp>
#include <boost/statistics/detail/distribution_common/meta/random/distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace meta{

    // U = V or U = V& allowed where V models UniformRandomNumberGenerator
    template<typename D,typename U>
    struct random_generator{
        typedef meta::random_distribution<D> meta_;
        typedef typename meta_::type r_;
        typedef boost::variate_generator<U,r_> type;
        
        static type call(U urng,const D& d){
            return type(urng,make_random_distribution(d));
        }
    };
    
}// meta

    template<typename D,typename U>
    typename meta::random_generator<D,U&>::type 
    make_random_generator(U& urng,const D& dist){
        typedef meta::random_generator<D,U&> meta_;
        return meta_::call(urng,dist);
    }

}// distribution
}// detail
}// statistics
}// boost

#endif
