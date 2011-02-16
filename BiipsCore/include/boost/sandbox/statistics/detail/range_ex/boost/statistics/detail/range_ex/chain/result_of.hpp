///////////////////////////////////////////////////////////////////////////////
// range_ex::chain::result_of.hpp                              				 //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_RANGE_EX_CHAIN_RESULT_OF_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_RANGE_EX_CHAIN_RESULT_OF_HPP_ER_2010
#include <boost/typeof/typeof.hpp>
#include <boost/range/chain.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace range{

	namespace result_of{

    	template<typename R1,typename R2>
		struct chain{

			// This version rather than the simpler BOOST_TYPEOF_TYPE
            // is needed for MSVC.
    		typedef BOOST_TYPEOF_NESTED_TYPEDEF_TPL(
            	nested,
        		boost::chain(
            		R1(),
                	R2()
            	)
        	);
            
            typedef typename nested::type type;
                
    		//typedef BOOST_TYPEOF_TPL(
        	//	boost::chain(
            //		R1(),
            //    	R2()
            //	)
        	//) type;
		};

	}

}// range
}// detail
}// statistics
}// boost

#endif