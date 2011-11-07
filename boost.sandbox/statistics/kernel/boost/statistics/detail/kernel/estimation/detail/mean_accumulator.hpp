///////////////////////////////////////////////////////////////////////////////
// kernel::functional::detail::mean_accumulator.hpp                          //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
/////////////////////////////////////////////////////////////////////////////// 
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_DETAIL_MEAN_ACCUMULATOR_H_ER_2009
#define BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_DETAIL_MEAN_ACCUMULATOR_H_ER_2009
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{
namespace detail{

    // The Rosenblatt-Parzen estimator is a density estimator
    template<typename T>
    struct mean_accumulator{
        typedef accumulators::stats<
            accumulators::tag::mean
        >  stat_;
        typedef accumulators::accumulator_set<T,stat_> type;
    };
        
}// detail
}// kernel
}// detail
}// statistics
}// boost

#endif