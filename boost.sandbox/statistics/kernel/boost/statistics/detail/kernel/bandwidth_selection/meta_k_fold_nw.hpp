////////////////////////////////////////////////////////////////////////////////
// kernel::bandwidth_selection::meta_k_fold_nw.hpp                            //
//                                                                            //
//  (C) Copyright 2009 Erwann Rogard                                          //
//  Use, modification and distribution are subject to the                     //
//  Boost Software License, Version 1.0. (See accompanying file               //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)          //
////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_BANDWIDTH_SELECTION_META_K_FOLD_NW_HPP_ER_2009           
#define BOOST_STATISTICS_DETAIL_KERNEL_BANDWIDTH_SELECTION_META_K_FOLD_NW_HPP_ER_2009
#include <boost/statistics/detail/kernel/bandwidth_selection/detail/k_fold.hpp>
#include <boost/statistics/detail/kernel/estimation/detail/mean_accumulator.hpp>
#include <boost/statistics/detail/kernel/estimation/meta_nw_visitor_unary.hpp>
#include <boost/statistics/detail/cross_validation/extractor/identity.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{
namespace bandwidth_selection{

template<
    typename F1,    // x extractor
    typename F2     // y extractor
>
struct meta_k_fold_nw
{

    typedef meta_nw_visitor_unary<F1,F2> meta_;

    // ft(u) has to return a training data. Here, ft(u) = (x,y),
    // f1((x,y))=x, f2((x,y)) = y
    
    template<
        typename U, // data-unit 
        typename K,
        typename Ft = cross_validation::extractor::identity,
        typename A = typename 
            kernel::detail::mean_accumulator<
                typename K::result_type>::type
    >
    struct apply{
        typedef bandwidth_selection::detail::k_fold<
            U,
            meta_::template apply,
            K,
            Ft,F1,F2
        > type;
    };

};
    
}// bandwidth_selection
}// kernel
}// detail
}// statistics
}// boost   

#endif

