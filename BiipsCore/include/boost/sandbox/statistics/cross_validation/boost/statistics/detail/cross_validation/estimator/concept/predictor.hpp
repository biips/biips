//////////////////////////////////////////////////////////////////////////////
// cross_validation::estimator::concept::predictor.hpp                      //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ESTIMATOR_CONCEPT_PREDICTOR_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ESTIMATOR_CONCEPT_PREDICTOR_HPP_ER_2009
#include <boost/concept_check.hpp>
#include <boost/statistics/detail/cross_validation/estimator/meta/estimator.hpp> 
#include <boost/statistics/detail/cross_validation/estimator/meta/input_data.hpp> 

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace estimator{
namespace concept{

    template <typename B>
    class Predictor : public B 
    {        
        typedef typename B::estimator_type  e_;
        typedef typename B::input_data_type i_;

        public:
        
        BOOST_CONCEPT_USAGE(Predictor)
        {
            e.predict( i ); 
        }
    
    private:
        e_ e;
        i_ i;
        
    };

}// concept
}// estimator
}// cross_validation
}// detail
}// statistics
}// boost

#endif