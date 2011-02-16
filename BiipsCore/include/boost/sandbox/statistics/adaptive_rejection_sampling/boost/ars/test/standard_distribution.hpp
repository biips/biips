///////////////////////////////////////////////////////////////////////////////
// ars::test::standard_distribution.hpp                                      //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_TEST_STANDARD_DISTRIBUTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_TEST_STANDARD_DISTRIBUTION_HPP_ER_2009

#include <iostream>
#include <list>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>

#include <boost/format.hpp>
#include <boost/range.hpp>
#include <boost/accumulators/accumulators.hpp>

#include <boost/statistics/detail/distribution_common/meta/random/generator.hpp>
#include <boost/statistics/detail/distribution_common/meta/value.hpp>
#include <boost/statistics/detail/non_parametric/kolmogorov_smirnov/statistic.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/ref_distribution.hpp>

#include <boost/ars/function/adaptor.hpp>
#include <boost/ars/constant.hpp>
#include <boost/ars/proposal_sampler.hpp>
#include <boost/ars/sampler.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{
namespace test{

struct standard_distribution{

	static void header(std::ostream& os){
    	os << "This test outputs a sequence {(a,b,c)} where" << std::endl;
    	os 	<< "a : sample size" << std::endl
     		<< "b : kolmogorov-smirnov statistic" << std::endl
     		<< "c : avg number of rejections" << std::endl;
    }

	// Samples from distribution D using adaptive rejection sampling and outputs
    // convergence statistics
	template<typename D,typename U>
	static void call(
    	const D& mdist, //e.g. D == math::normal_distribution<T>
    	typename D::value_type x_min,
    	typename D::value_type x_max,
    	typename D::value_type init_0,
    	typename D::value_type init_1,
    	U& urng,
    	unsigned n1,   // # loops
    	unsigned n2,   // # subsamples per loop  
    	unsigned n3,   // size of subsample 
    	unsigned n4,   // At each loop, n2 *= n4
    	unsigned n_max_reject,
    	std::ostream& os
	){

    	// The ars is re-initialized after each n3 sample.
    	// n3 * n4 is the total size of the sample over which a KS is computed

    	namespace dist = boost::statistics::detail::distribution;
    	namespace ks = boost::statistics::detail::kolmogorov_smirnov;
    	typedef std::string                                     str_;
    	typedef std::runtime_error                              err_;
        typedef typename D::value_type							val_;
    	typedef std::vector<val_>                               vals_;

    	typedef ars::function::adaptor<D>                       fun_t;
    	typedef ars::proposal_sampler<val_,std::vector>         ps_;
    	typedef ars::sampler<ps_>                               ars_;
    	typedef random::ref_distribution<ars_&>                 ref_ars_;
    	typedef variate_generator<U&,ref_ars_>                  vg_ars_;

    	typedef ks::tag::statistic<val_> 						tag_ks_;
    	typedef boost::accumulators::stats<tag_ks_> 			acc_features_;
    	typedef boost::accumulators::accumulator_set<val_,acc_features_> acc_;

    	ars_ ars;
    	ars.set_function(x_min, x_max, fun_t(mdist));
    	{
    		static const str_ str 
        		= "Initialized every %1% draw(s) with x1 = %2% and x2 = %3%";
        	boost::format f(str);
        	f%n3%init_0%init_1;
        	os << f.str() << std::endl;
        	os << description(mdist) << std::endl;
    	}
    	long unsigned n_reject;
    
    	try{
        	for(unsigned i1 = 0; i1<n1; i1++){
            	n_reject = 0;
            	acc_ acc;
            	for(unsigned i2 = 0; i2<n2; i2++){
                	try{
                    	ars.initialize(init_0,init_1);
                    	vg_ars_ vg_ars(urng,ref_ars_(ars)); 
                    	for(unsigned i = 0; i<n3; i++)
                    	{
							val_ x = vg_ars();
							acc(x);
                    	}
                    	// Without ref_ars_, n would be reset to 0
                    	n_reject += (
                        	(vg_ars.distribution()).distribution()
                    	).n_reject();
                	}catch(std::exception& e){
                    	boost::format f("at i1 = %1%, i2 = %2% : %3%"); 
                    	f % i1 % i2 % e.what();
                    	throw std::runtime_error(f.str());
                	}
            	}
            	long int n = boost::accumulators::extract::count(acc);
            	val_ st = ks::extract::statistic<val_>(acc,mdist);
            	val_ rate = static_cast<val_>(n_reject)/static_cast<val_>(n3*n2);
            	os 
                	<< '(' 
                	<< n
                	<< ','
                	<< st
                	<< ','
                	<< rate 
                	<< ')'
                	<< std::endl;
            	n2 *= n4;
        	}
    	}catch(std::exception& e)
    	{
        	std::cerr << e.what() << std::endl;
    	}
    	os << std::endl;
	}
};

}//test
}// ars
}// detail
}// statistics
}// boost

#endif


