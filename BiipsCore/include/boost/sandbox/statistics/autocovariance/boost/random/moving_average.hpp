///////////////////////////////////////////////////////////////////////////////
// moving_average.hpp                                                        //
//                                                                           //
//  Copyright 2008 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ACCUMULATORS_RANDOM_MOVING_AVERAGE_HPP_ER_2008_04
#define BOOST_ACCUMULATORS_RANDOM_MOVING_AVERAGE_HPP_ER_2008_04

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/fir.hpp>
#include <boost/range/iterator_range.hpp>
namespace boost{namespace random
{

// Simulates a Moving average process
template<typename T>
class moving_average{
public:
    typedef T               input_type;
    typedef T               result_type ;
    template<typename R>
    moving_average(const R& coeffs)
        :acc(
            accumulators::tag::fir::coefficients=coeffs,
            accumulators::tag::delay<>::cache_size=coeffs.size()){}
    moving_average(const moving_average& that)
        :acc(that.acc){}
    moving_average& operator=(const moving_average& that){
        if(&that!=this){
            acc = that.acc;
        }
        return *this;
    }
    template<typename G>//G models NumberGenerator
    result_type operator()(G& gen){
        T x = gen();
        acc(x);
        return accumulators::extract::fir(acc);
    }
private:
    typedef accumulators::accumulator_set<
        T,
        accumulators::stats<
            accumulators::tag::fir,
            accumulators::tag::delay<>
        >
    > acc_type;
    acc_type acc;
};

}}
#endif
