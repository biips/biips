///////////////////////////////////////////////////////////////////////////////
// range_ex::chain::nest.hpp                              				 	 //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_RANGE_EX_CHAIN_NEST_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_RANGE_EX_CHAIN_NEST_HPP_ER_2010
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/eval_if.hpp>

#include <boost/utility/enable_if.hpp>

#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/range.hpp>

#include <boost/fusion/include/back.hpp>
#include <boost/fusion/include/pop_back.hpp>
#include <boost/fusion/include/size.hpp>

#include <boost/statistics/detail/range_ex/chain/result_of.hpp>
#include <boost/statistics/detail/range_ex/chain/detail/nest.hpp>
#include <boost/statistics/detail/range_ex/chain/detail/dont_nest.hpp>
#include <boost/statistics/detail/range_ex/chain/detail/result_of_nest.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace range{

namespace result_of{
	
	template<typename Seq>
    struct nest_chain : boost::mpl::eval_if<
    	range::impl::dont_nest<Seq>,
		range::impl::result_of::first<Seq>,
        range::impl::result_of::nest_chain<Seq>
    >{};

}// result_of


	// Usage for n=1
    // nest_chain(fusion::make_vector(vec1)); 	
    template<typename Seq>
    typename boost::lazy_enable_if<
    	range::impl::dont_nest<Seq>,
    	range::impl::result_of::first<Seq>
    >::type
    nest_chain(const Seq& seq){
		typedef range::impl::result_of::first<Seq> meta_;
        return meta_::call(seq);
	}

	// Usage for n>1
    // nest_chain(fusion::make_vector(vec1,vec2,vec3,...vec_n)); 	
    template<typename Seq>
    typename boost::lazy_disable_if<
    	range::impl::dont_nest<Seq>,
    	range::impl::result_of::nest_chain<Seq>
    >::type
    nest_chain(const Seq& seq){
    	typedef typename boost::fusion::result_of::size<Seq>::type size_;
        typedef boost::mpl::int_<1> int1_;
    
    	BOOST_MPL_ASSERT(( boost::mpl::greater<size_,int1_>));
    
    	typedef typename 
        	boost::fusion::result_of::pop_back<Seq>::type cref_most_;
        typedef typename boost::remove_const<
        	typename boost::remove_reference<
            	cref_most_
            >::type
        >::type most_;
    	typedef typename boost::fusion::result_of::back<Seq>::type cref_back_;
        typedef typename boost::remove_const<
        	typename boost::remove_reference<
            	cref_back_
            >::type
        >::type back_;
    
    	typedef range::impl::nest_chain<most_,back_> meta_;
		return meta_::call(
        	boost::fusion::pop_back(seq),
            boost::fusion::back(seq)
        );    
    }

}// range
}// detail
}// statistics
}// boost

#endif
