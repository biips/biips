//////////////////////////////////////////////////////////////////////////////
// cross_validation::estimator::adaptor::joined.hpp                         //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ESTIMATOR_ADAPTOR_JOINED_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ESTIMATOR_ADAPTOR_JOINED_HPP_ER_2009
//#include <boost/mpl/empty_base.hpp>
#include <boost/statistics/detail/mpl/nested_type.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/statistics/detail/cross_validation/estimator/adaptor/wrapper.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace estimator{
namespace adaptor{

    // A/a and T/t type/object pairs for accumulator an training value.
    // Expression          
    // a(x)              
    template<
        template<typename> class T, // trainer
        template<typename> class P, // predictor
        typename A
    >
    class joined : public   
        mpl::nested_type<
            T< 
                typename mpl::nested_type< 
                    P< 
                        wrapper<A> 
                    > 
                >::type
            >
        >::type
        {
        typedef wrapper<A> w_;
        typedef typename mpl::nested_type< 
            P< 
                wrapper<A> 
            > 
        >::type p_;
        typedef typename mpl::nested_type<
            T< 
                p_
            >
        >::type  t_;      
        public:
        
        joined(A& a): t_(
            p_(
                w_(a)
            )
        ){}
        joined(const joined& that): t_(that){}
    
        private:
        joined();
    };

}// adaptor
}// estimator
}// cross_validation
}// detail
}// statistics
}// boost

#endif