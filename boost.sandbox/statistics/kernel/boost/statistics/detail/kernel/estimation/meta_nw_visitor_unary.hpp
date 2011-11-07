///////////////////////////////////////////////////////////////////////////////
// kernel::functional::meta_nw_visitor_unary.hpp                             //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
/////////////////////////////////////////////////////////////////////////////// 
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_META_NW_VISITOR_UNARY_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_META_NW_VISITOR_UNARY_HPP_ER_2009
#include <boost/statistics/detail/kernel/estimation/detail/mean_accumulator.hpp>
#include <boost/statistics/detail/kernel/estimation/detail/nw_visitor_unary.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{

// meta_visitor_unary<F1,F2>::apply is a valid template argument to estimator
// See detail/nw_visitor_unary
template<
    typename F1,    // x extractor
    typename F2
>
struct meta_nw_visitor_unary
{
    
     template<
        typename K,
        typename X,
        typename A = typename statistics::detail::kernel::detail::mean_accumulator<
            typename K::result_type
        >::type 
    >
     struct apply{
       typedef detail::nw_visitor_unary<K,X,F1,F2,A> type;
    };
};

}// kernel
}// detail
}// statistics
}// boost

#endif
