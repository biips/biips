///////////////////////////////////////////////////////////////////////////////
// range_ex::chain::detail::nest.hpp                              		     //
//                                                                           //
//  Copyright 2008 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_RANGE_EX_CHAIN_DETAIL_NEST_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_RANGE_EX_CHAIN_DETAIL_NEST_HPP_ER_2010
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/greater.hpp>

#include <boost/type_traits.hpp>

#include <boost/fusion/include/back.hpp>
#include <boost/fusion/include/pop_back.hpp>
#include <boost/fusion/include/size.hpp>

#include <boost/statistics/detail/range_ex/chain/result_of.hpp>
#include <boost/statistics/detail/range_ex/chain/detail/nest.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace range{

namespace impl{
    
	template<
    	typename Most,
        typename Rhs,
        bool exit = boost::mpl::equal_to<
            typename boost::fusion::result_of::size<Most>::type,
            boost::mpl::int_<1>
        >::value
    >
	struct nest_chain{
        // exit = Most has size one
    };

	template<typename Most,typename Rhs>
	struct nest_chain<Most,Rhs,true>
	{

    	BOOST_MPL_ASSERT_NOT((boost::is_reference<Most>));
    	BOOST_MPL_ASSERT_NOT((boost::is_reference<Rhs>));
        
    	typedef Most most_;
    
    	typedef 
        	typename boost::fusion::result_of::back<most_>::type cref_back_;

        typedef typename boost::remove_const<
        	typename boost::remove_reference<
            	cref_back_
            >::type
        >::type back_;

    	typedef typename statistics::detail::range::result_of::chain<
			back_,
            Rhs        	
        >::type type;
    
        // TODO Why Most1!= Most
    	template<typename Most1>
        static type call(const Most1& most, const Rhs& rhs)
        {
            return boost::chain(
            	boost::fusion::back(most),rhs
            );
        }
	};

	template<typename Most,typename Rhs>
	struct nest_chain<Most,Rhs,false>
	{
    	BOOST_MPL_ASSERT_NOT((boost::is_reference<Most>));
    	BOOST_MPL_ASSERT_NOT((boost::is_reference<Rhs>));
        typedef boost::mpl::int_<1> int1_;
        typedef typename boost::fusion::result_of::size<Most>::type size_most_;
        BOOST_MPL_ASSERT((
			boost::mpl::greater<
        		size_most_,
            	int1_
        	>
        ));

    	typedef 
        	typename boost::fusion::result_of::pop_back<Most>::type
        cref_next_most_;
            
		typedef typename boost::remove_const<
        	typename boost::remove_reference<
            	cref_next_most_
            >::type
        >::type next_most_;

    	typedef 
        	typename boost::fusion::result_of::back<Most>::type cref_back_;
        typedef typename boost::remove_const<
        	typename boost::remove_reference<cref_back_>::type
        >::type back_;

        typedef typename statistics::detail::range::result_of::chain<
            back_,
            Rhs
        >::type next_rhs_;
    
        typedef 
        	range::impl::nest_chain<next_most_,next_rhs_> 
        next_impl;

        typedef typename next_impl::type type;

		template<typename Most1>
        static type call(const Most1& most, const Rhs& rhs)
        {

            return next_impl::call(
            	boost::fusion::pop_back(most),
                boost::chain(
                	boost::fusion::back(most),
                    rhs
                )
            );
        }
	};

}// impl

}// range
}// detail
}// statistics
}// boost

#endif


