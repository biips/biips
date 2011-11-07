///////////////////////////////////////////////////////////////////////////////
// range_ex::chain::detail::dont_nest.hpp                               	 //
//                                                                           //
//  Copyright 2008 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_RANGE_EX_CHAIN_DETAIL_DONT_NEST_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_RANGE_EX_CHAIN_DETAIL_DONT_NEST_HPP_ER_2010
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>

#include <boost/type_traits.hpp>
#include <boost/range.hpp>

#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/size.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace range{
namespace impl{

	template<typename Seq>
	struct dont_nest :
    	boost::mpl::equal_to<
            typename boost::fusion::result_of::size<Seq>::type,
            boost::mpl::int_<1>
        >{};

namespace result_of{

	// Case dont_nest
    template<typename Seq>
    struct first{
        typedef typename boost::fusion::result_of::at_c<const Seq,0>::type type;
        type static call(const Seq& seq){
			return boost::fusion::at_c<0>(seq);
        }
    };

}// result_of

}// impl
}// range
}// detail
}// statistics
}// boost

#endif
    