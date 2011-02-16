//////////////////////////////////////////////////////////////////////////////
// cross_validation::k_fold::initialize.hpp                                 //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_K_FOLD_INITILIAZE_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_K_FOLD_INITILIAZE_HPP_ER_2009
#include <algorithm>
#include <boost/range.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/statistics/detail/cross_validation/k_fold/train_predict.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace k_fold{ 

    template<
        typename U,typename Ft,typename Fi,typename Fo,
        typename E,typename It,typename It1
    >
    It cross_validate(
        partition<U,Ft,Fi,Fo>& kfp,
        E estimator,
        It  i_p,    // predicted values
        It1 i_o     // true values
    ){
        kfp.initialize();
        while(kfp.index()<kfp.n_folds()){
            i_p = train_predict(
                kfp,
                estimator,
                i_p
            );
            
            BOOST_AUTO(
                r,
                kfp.output_range()
            );
            
            i_o = std::copy(
                boost::begin(r),
                boost::end(r),
                i_o
            );

            kfp.increment();
        }
        return i_p;
    }

}// k_fold
}// cross_validation
}// detail
}// statistics
}// boost

#endif

