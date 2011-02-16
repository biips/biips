///////////////////////////////////////////////////////////////////////////////
// distribution::common::meta::random::distribution.hpp                      //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_RANDOM_DISTRIBUTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_RANDOM_DISTRIBUTION_HPP_ER_2009
#include <boost/utility/enable_if.hpp>
#include <boost/statistics/detail/distribution_toolkit/meta/is_scalar_distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace meta{

    template<typename D>
    struct random_distribution{};

    // Provide a specialization for each D that models RandomDistribution 
    // http://www.boost.org/doc/libs/1_40_0/libs/random/random-concepts.html
    // and provide static type call(const D& )

}// meta

	template<typename D>
    typename meta::random_distribution<D>::type
    make_random_distribution(const D& d){
        typedef meta::random_distribution<D> meta_;
        return meta_::call(d);
    }

}// distribution
}// detail
}// statistics
}// boost

#endif
