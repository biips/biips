///////////////////////////////////////////////////////////////////////////////
// random::categorical_distribution.hpp                                      //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_RANDOM_CATEGORICAL_DISTRIBUTION_HPP_ER_2009
#define BOOST_RANDOM_CATEGORICAL_DISTRIBUTION_HPP_ER_2009
#include <algorithm>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <boost/range.hpp>
#include <boost/format.hpp>
#include <boost/math/tools/precision.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/utility/result_of.hpp>

namespace boost{
namespace random{

// Deprecated : see discrete_distribution
//
// Usage:
// typedef categorical_distribution<> rmult_;
// typedef rmult_::value_type         value_type;
// typedef rmult_::result_type        idx_;
//
// Let urng model UniformRandomNumberGenerator and weights a sequence whose 
// elements type is convertible to value_type:
//
// rmult_ rmult(weights);
// idx_ idx = rmult(urng);
template<typename Ur = uniform_real<double> >
class categorical_distribution{
    typedef Ur                                          unif_t;
    public:
    typedef typename unif_t::input_type                 input_type;
    typedef typename unif_t::result_type                value_type;

    private:
    typedef std::vector<value_type>                     cont_t;

    public:
    typedef typename range_difference<cont_t>::type     result_type;

    // Construction
    categorical_distribution();
    
    // Passing sorted from large to small weights will speed up execution. 
    template<typename R>
    categorical_distribution( const R& unnormalized_weights );
    categorical_distribution( const categorical_distribution& that );
    categorical_distribution&
    operator=(const categorical_distribution& that);

    // Draw
    template<typename U> result_type operator()(U& urng)const;

    // Access
    const cont_t& cumulative_weights()const;
    value_type normalizing_constant()const;

    // TODO os/is

    private:
    cont_t cum_sums_;
    template<typename R>void set(const R& unnormalized_weights);
};


// Implementation //

    // Construction
    template<typename Ur>
    categorical_distribution<Ur>::categorical_distribution() : cum_sums_(){}

    template<typename Ur>
    template<typename R>
    categorical_distribution<Ur>::categorical_distribution( 
        const R& unnormalized_weights 
    )
    {
        set(unnormalized_weights);
    }
    
    template<typename Ur>
    categorical_distribution<Ur>::categorical_distribution( 
        const categorical_distribution& that 
    )
    :cum_sums_(that.cum_sums_){}
    
    template<typename Ur>
    categorical_distribution<Ur>&
    categorical_distribution<Ur>::operator=(
        const categorical_distribution& that
    ){
        if(&that!=this){
            this->cum_sums_ = that.cum_sums_;
        }
        return *this;
    }
    
    template<typename Ur>
    template<typename U>
    typename categorical_distribution<Ur>::result_type 
    categorical_distribution<Ur>::operator()(U& urng)const{
        unif_t unif(static_cast<value_type>(0),normalizing_constant());
        typedef typename range_iterator<const cont_t>::type iter_;
        value_type u = unif(urng);
        iter_ i = std::lower_bound(
            boost::begin(this->cum_sums_),
            boost::end(this->cum_sums_),
            u
        );
        BOOST_ASSERT(i!=end(cum_sums_));
        return std::distance(boost::begin(this->cum_sums_),i);
    }
    
    // Access
    template<typename Ur>
    const typename categorical_distribution<Ur>::cont_t&
    categorical_distribution<Ur>::cumulative_weights()const{ 
        return (this->cum_sums_); 
    }
    
    template<typename Ur>
    typename categorical_distribution<Ur>::value_type
    categorical_distribution<Ur>::normalizing_constant()const{
        return (this->cum_sums_).back();
    }
    
    template<typename Ur>
    template<typename R>
    void categorical_distribution<Ur>::set(const R& unnormalized_weights){
        const char* method = "multinormal_distribution::set, error : ";
        static value_type eps = math::tools::epsilon<value_type>();

        cum_sums_.resize(size(unnormalized_weights));
        
        std::partial_sum(
            boost::begin(unnormalized_weights),
            boost::end(unnormalized_weights),
            boost::begin(this->cum_sums_)
        );

        if(math::isinf(this->normalizing_constant())){
            std::string str = method;
            str += "isinf(nc)";  
            throw std::runtime_error(str);
        }
        if(this->normalizing_constant()<eps){
            std::string str = method;
            str += "nc = %1% < eps = %2%";
            format f(str); f%(this->normalizing_constant())%eps;  
            throw std::runtime_error(f.str());
        }
        
    }

}// random
}// boost

#endif 
