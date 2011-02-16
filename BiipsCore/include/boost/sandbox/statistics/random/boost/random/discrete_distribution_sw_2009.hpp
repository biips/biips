// discrete_distribution.hpp
//
// Copyright (c) 2009
// Steven Watanabe
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_RANDOM_DISCRETE_DISTRIBUTION_HPP_INCLUDED
#define BOOST_RANDOM_DISCRETE_DISTRIBUTION_HPP_INCLUDED

#include <vector>
#include <cassert>
#include <limits>
#include <numeric>
#include <utility>

#include <boost/range.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/variate_generator.hpp>

namespace boost {
namespace random {

template<class IntType, class WeightType>
class discrete_distribution {
public:
    typedef WeightType input_type;
    typedef IntType result_type;

    template<class Iter>
    discrete_distribution(Iter begin, Iter end) : weights(begin, end), data(weights.size()) {
        std::size_t size = weights.size();
        //assert(size <= (std::numeric_limits<IntType>::max)());
        std::vector<std::pair<WeightType, IntType> > below_average;
        std::vector<std::pair<WeightType, IntType> > above_average;
        WeightType weight_sum = std::accumulate(weights.begin(), weights.end(), static_cast<WeightType>(0));
        WeightType weight_average = weight_sum / size;
        for(std::size_t i = 0; i < size; ++i) {
            if(weights[i] < weight_average) {
                below_average.push_back(std::make_pair(weights[i] / weight_average, static_cast<IntType>(i)));
            } else {
                above_average.push_back(std::make_pair(weights[i] / weight_average, static_cast<IntType>(i)));
            }
        }
        typedef typename range_iterator< 
            std::vector<
                std::pair<WeightType, IntType> 
            >
        >::type iter_;
            iter_ b_iter = below_average.begin();
            iter_ b_end = below_average.end();
            iter_ a_iter = above_average.begin();
            iter_ a_end = above_average.end();
        while(b_iter != b_end && a_iter != a_end) {
            data[b_iter->second] = std::make_pair(b_iter->first, a_iter->second);
            a_iter->first -= (1 - b_iter->first);
            if(a_iter->first < 1) {
                *b_iter = *a_iter++;
            } else {
                ++b_iter;
            }
        }
        for(; b_iter != b_end; ++b_iter) {
            data[b_iter->second].first = 1;
        }
        for(; a_iter != a_end; ++a_iter) {
            data[a_iter->second].first = 1;
        }
    }
    template<class Engine>
    IntType operator()(Engine& eng) const {
        assert(!data.empty());
        boost::variate_generator<Engine&, boost::uniform_01<WeightType> > real_gen(eng, boost::uniform_01<WeightType>());
        WeightType test = real_gen() * data.size();
        IntType result = static_cast<IntType>(test);
        if(test - result < data[result].first) {
            return result;
        } else {
            return(data[result].second);
        }
    }
    
    result_type min BOOST_PREVENT_MACRO_SUBSTITUTION () const { return 0; }
    result_type max BOOST_PREVENT_MACRO_SUBSTITUTION () const { return static_cast<result_type>(weights.size() - 1); }
private:
    std::vector<WeightType> weights;
    std::vector<std::pair<WeightType, IntType> > data;
};

}
}

#endif


