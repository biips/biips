//////////////////////////////////////////////////////////////////////////////////////////
// kernel::bandwidth_selection::detail::cross_validate.hpp                              //
//                                                                                      //
//  (C) Copyright 2009 Erwann Rogard                                                    //
//  Use, modification and distribution are subject to the                               //
//  Boost Software License, Version 1.0. (See accompanying file                         //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)                    //
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_BANDWIDTH_SELECTION_DETAIL_K_FOLD_HPP_ER_2009           
#define BOOST_STATISTICS_DETAIL_KERNEL_BANDWIDTH_SELECTION_DETAIL_K_FOLD_HPP_ER_2009
#include <boost/statistics/detail/kernel/estimation/estimator.hpp>
#include <boost/statistics/detail/kernel/estimation/detail/mean_accumulator.hpp>
#include <boost/statistics/detail/cross_validation/k_fold/partition.hpp>
#include <boost/statistics/detail/cross_validation/k_fold/cross_validate.hpp>
#include <boost/statistics/detail/cross_validation/k_fold/train_predict.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{
namespace bandwidth_selection{
namespace detail{

    template<
        typename U, // data-unit 
        template<typename,typename,typename> class V,
        typename K,
        typename Ft,
        typename Fi,
        typename Fo,
        typename A = typename 
            kernel::detail::mean_accumulator<typename K::result_type>::type
    >
    class k_fold : 
        public cross_validation::k_fold::partition<U,Ft,Fi,Fo>
    {
        
        typedef cross_validation::k_fold::partition<U,Ft,Fi,Fo>     kfp_;
        typedef typename kfp_::meta_training_range::type            tr_;
        typedef estimator<tr_,V,K,A>                                est_t;
        typedef kfp_& rkfp_;
        
        public:
        typedef kfp_ super_;
        typedef est_t estimator_type;

        // Constructor
        k_fold(){}
        template<typename It>
        k_fold(
            unsigned num_folds, 
            It b,   //dataset
            It e
        ):kfp_(
            num_folds,
            b,e
        ){
            // Default constructs estimator
        }

        // TODO create a constructor with argument pack (overloading constr
        // uctor would be cumbersome)

        // Access

        // Cross validation
        
        template<typename T,typename It>
        It train_predict(
            K k,
            It  it_p // predicted values
        )
        {
            rkfp_ rkfp = static_cast<rkfp_>(*this);
        
            return cross_validation::k_fold::train_predict(
                rkfp,
                est_t(k),
                it_p
            );
        }

        template<typename It,typename It1>
        It cross_validate(
            K k,
            It i_p,     // predicted values
            It1 i_o    // true values
        ){
            rkfp_ rkfp = static_cast<rkfp_>(*this);
            
            return cross_validation::k_fold::cross_validate(
                rkfp,
                est_t(k),
                i_p,
                i_o
            );
        }
    
    };

    
}// detail
}// bandwidth_selection
}// kernel
}// detail
}// statistics
}// boost   

#endif