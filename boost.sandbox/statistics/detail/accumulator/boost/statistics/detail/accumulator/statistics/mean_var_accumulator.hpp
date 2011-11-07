///////////////////////////////////////////////////////////////////////////////
// statistics::detail::accumulator::statistics::mean_var_accumulator.hpp     //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ACCUMULATOR_STATISTICS_MEAN_VAR_ACCUMULATOR_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ACCUMULATOR_STATISTICS_MEAN_VAR_ACCUMULATOR_HPP_ER_2009
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/stats.hpp>

namespace boost{
namespace functional{

    // This comes up often, so this saves a bit of time
    template<typename T>
    struct mean_var_accumulator{
        typedef accumulators::stats<
            accumulators::tag::mean,
            accumulators::tag::variance
        >  stat_;
        typedef accumulators::accumulator_set<T,stat_> type;
    };

}// meta    
}// boost

#endif
