//////////////////////////////////////////////////////////////////////////////
// cross_validation::estimator::adaptor::unary_trainer.hpp                  //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ESTIMATOR_ADAPTOR_UNARY_TRAINER_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ESTIMATOR_ADAPTOR_UNARY_TRAINER_HPP_ER_2009
//#include <boost/mpl/empty_base.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/call_traits.hpp>
#include <boost/statistics/detail/cross_validation/estimator/adaptor/wrapper.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace estimator{
namespace adaptor{

    // Adapts a unary functor to model Predictor
    //
    // B->reference() must return an unary functor
    template<typename B>
    class unary_trainer : public B{
        public:
        
        unary_trainer(const B& b): B(b){}
        unary_trainer(const unary_trainer& that): B(that){}
    
        template<typename R>
        void train(R training_range){
            typedef typename range_value<R>::type t_;
            typedef typename call_traits<t_>::param_type par_;
        
            BOOST_FOREACH(par_ t,training_range){
                this->accumulate(t);
            }
        
        }

        private:
        template<typename M>
        void accumulate(const M& m)const{
            this->reference()(m);
        }
        unary_trainer();
    };

}// adaptor
}// estimator
}// cross_validation
}// detail
}// statistics
}// boost

#endif