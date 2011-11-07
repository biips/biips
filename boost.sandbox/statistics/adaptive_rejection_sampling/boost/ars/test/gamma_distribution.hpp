///////////////////////////////////////////////////////////////////////////////
// ars::test::gamma_distribution.cpp                                         //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_TEST_GAMMA_DISTRIBUTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_TEST_GAMMA_DISTRIBUTION_HPP_ER_2009
#include <iostream>
#include <string>
#include <boost/random/mersenne_twister.hpp>
#include <boost/statistics/detail/distribution_toolkit/distributions/gamma/include.hpp>
#include <boost/ars/test/standard_distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{
namespace test{

struct gamma_distribution{

    // Samples from gamma distribution using adaptive rejection sampling and 
    // outputs convergence statistics
    template<typename T>
	static void call(
    	T shape,  // must be > 1
    	T scale,
    	T init_0, // must be >0 and < init_1
    	T init_1, // must be > mode = (m-1) * theta
    	unsigned n1,    // 1e2
    	unsigned n2,    // 10
    	unsigned n3,    // 1
    	unsigned n4,    // 10
    	unsigned n_max_reject,
    	std::ostream& out
	)
	{

    	using namespace boost;
    	typedef double                                          val_;
    	typedef ars::constant<val_>                             const_;
    	typedef math::gamma_distribution<val_>                  mdist_t;
    	typedef boost::mt19937                                  urng_;

    	mdist_t mdist(shape,scale);
    	urng_ urng;

    	const val_ inf_ = const_::inf_;

    	ars::test::standard_distribution::call(
        	mdist,
        	static_cast<val_>(0),   // x_min
        	inf_,                   // x_max
        	init_0,                 
        	init_1, 
        	urng,   
        	n1,    
        	n2,    
        	n3,    
        	n4,    
        	n_max_reject,
        	out
    	);
	}
};

}//test
}// ars
}// detail
}// statistics
}// boost


#endif
