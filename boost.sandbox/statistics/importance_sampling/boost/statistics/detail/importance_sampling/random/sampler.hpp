///////////////////////////////////////////////////////////////////////////////
// importance_sampling::sampler.hpp                                          //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_SAMPLER_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_SAMPLER_HPP_ER_2009
#include <vector>
#include <boost/mpl/not.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/call_traits.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility.hpp>
#include <boost/range.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_real.hpp>
// TODO #include <boost/random/discrete_distributionhpp> when becomes avail
#include <boost/random/discrete_distribution_sw_2009.hpp> 

namespace boost{
namespace statistics{
namespace detail{
namespace importance_sampling{

// Samples by SIR given a set of proposal values and their unnormalized weights
//
// Models RandomDistribution
//
// R1:  type of a range values 
// W:   type of each weight
template<typename R1,typename W>
class sampler{
    typedef typename remove_reference<R1>::type         const_values_;
    typedef typename remove_cv<const_values_>::type     values_t;    
    typedef typename range_size<values_t>::type         size_;
    typedef is_reference<R1>                            is_ref_;
    public:
    typedef typename range_value<const_values_>::type   result_type;
    private:
    typedef random::discrete_distribution<size_,W>      discr_dist_t;
    public:
    typedef typename discr_dist_t::input_type           input_type;

    sampler(){}
    template<typename R0>
    sampler(
        const R0& unnormalized_weights,
        typename call_traits<R1>::param_type values
    ):discr_dist_(
        boost::begin(unnormalized_weights),
        boost::end(unnormalized_weights)
    ),values_(values){
        BOOST_ASSERT(
            boost::size(unnormalized_weights) == boost::size(this->values())
        );
    }
    
    sampler(const sampler& that)
        :discr_dist_(that.discr_dist_),values_(that.values_){}
    
    sampler& operator=(const sampler& that)
    {
        if(&that!=this){
            discr_dist_ = that.discr_dist_;
            values_ = that.values_;
        }
        return (*this);
    }

    template<typename U> 
    result_type operator()(U& urng)const
    {
        typedef typename discr_dist_t::result_type k_t;
        k_t k = discr_dist_(urng);
        BOOST_ASSERT( k < boost::size(this->values()) );
        return (*boost::next(boost::begin(this->values()),k));
    }    
    const discr_dist_t& discrete_distribution()const
    {
        return this->discr_dist_;     
    }

    // TODO os/is

    typename call_traits<R1>::const_reference values()const{
        return this->values_;
    }

    private:
    discr_dist_t discr_dist_;
    typename call_traits<R1>::value_type values_;
};

    template<typename R0,typename R1>
    sampler<
        R0,
        typename remove_cv<
            typename remove_reference<
                typename range_value<R0>::type
            >::type
        >::type
    >
    make_sampler(
        const R0& unnormalized_weights,
        typename call_traits<R1>::param_type values
    )
    {
        typedef sampler<
            R0,
            typename remove_cv<
                typename remove_reference<
                    typename range_value<R0>::type
                >::type
            >::type
        > result_;
        return result_(unnormalized_weights,values);
    }

}// importance_sampling
}// detail
}// statistics
}// boost

#endif 