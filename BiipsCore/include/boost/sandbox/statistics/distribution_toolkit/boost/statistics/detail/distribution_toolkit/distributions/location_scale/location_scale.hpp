//////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::location_scale::location_scale.hpp  //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_LOCATION_SCALE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_LOCATION_SCALE_HPP_ER_2009
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/mpl/assert.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits.hpp> // remove_reference etc.
#include <boost/range.hpp>
#include <boost/statistics/detail/distribution_common/meta/inherit/policy.hpp>
#include <boost/statistics/detail/distribution_toolkit/meta/is_scalar_distribution.hpp>
#include <boost/statistics/detail/distribution_common/meta/value.hpp>
#include <boost/statistics/detail/distribution_common/meta/policy.hpp>
#include <boost/math/policies/policy.hpp> // probably not needed
#include <boost/math/distributions/detail/common_error_handling.hpp>
#include <boost/math/policies/error_handling.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{


    // Represents a location scale family
    //
    // X = mu + sigma * Z
    // pdf_X(x) = (1/sigma) pdf_Z((x-mu)/sigma)
    // cdf_X(x) = cdf_Z((x-mu)/sigma)
    //
    // TODO: range and support
    template<typename Z>
    class location_scale_distribution{
    	typedef std::string str_;
        static const str_ fun;
        
        public:

        typedef Z distribution_type;

        BOOST_MPL_ASSERT(( meta::is_scalar_distribution<Z> ));

        typedef typename distribution::meta::value<
            distribution_type
        >::type value_type;

        typedef typename distribution::meta::policy<
            distribution_type
        >::type policy_type;

        location_scale_distribution():mu_(0),sigma_(1),z_(){}
        location_scale_distribution(
            value_type mu,
            value_type sigma,
            const Z& z
        ):mu_(mu),sigma_(sigma),z_(z){
        	static const str_ str 
            	= (fun + "<%1%>::location_scale_distribution").c_str();
            
            // TODO at present %1% is replaced by value_type. Write a function
            // that is implemented in terms of check_scale but actually puts
            // Z in its place
            
        	value_type result;
       	 	boost::math::detail::check_location(
            	str.c_str(), mu, &result, policy_type());
       	 	boost::math::detail::check_scale(
            	str.c_str(), sigma, &result, policy_type());
    	}        

        location_scale_distribution(const location_scale_distribution& that)
        :mu_(that.mu_),sigma_(that.sigma_),z_(that.z_){}
        
        location_scale_distribution&
        operator=(const location_scale_distribution& that){
        	if(&that!=this){
            	mu_ = that.mu_;
            	sigma_ = that.sigma_;
            	z_ = that.z_;
       	 	}
        	return *this;
    	}
        
        value_type mu()const{ return this->mu_; }
        value_type sigma()const{ return this->sigma_; }
        const Z& z()const{ return this->z_; }

        private:
        value_type mu_;
        value_type sigma_;
        Z z_;
    };
    template<typename Z>
    const std::string location_scale_distribution<Z>::fun 
    	= "distribution::toolkit::location_scale_distribution";


}// toolkit
}// distribution
}// detail
}// statistics
}// boost

#endif 
