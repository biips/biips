///////////////////////////////////////////////////////////////////////////////
// range_ex::chain::detail::result_of_nest.hpp                               //
//                                                                           //
//  Copyright 2008 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_RANGE_EX_CHAIN_DETAIL_RESULT_OF_NEST_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_RANGE_EX_CHAIN_DETAIL_RESULT_OF_NEST_HPP_ER_2010
#include <boost/type_traits.hpp>

#include <boost/fusion/include/back.hpp>
#include <boost/fusion/include/pop_back.hpp>
#include <boost/fusion/include/size.hpp>

#include <boost/statistics/detail/range_ex/chain/detail/nest.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace range{
namespace impl{
namespace result_of{
    
    // Case size(seq) > 1
    template<typename Seq>
    struct nest_chain : range::impl::nest_chain<
        typename boost::remove_cv<
            typename boost::remove_reference<
                typename boost::fusion::result_of::pop_back<Seq>::type
            >::type
        >::type
        ,
        typename boost::remove_cv<
            typename boost::remove_reference<
                typename boost::fusion::result_of::back<Seq>::type
            >::type
        >::type
    >{};

}// result_of
}// impl
}// range
}// detail
}// statistics
}// boost

#endif
