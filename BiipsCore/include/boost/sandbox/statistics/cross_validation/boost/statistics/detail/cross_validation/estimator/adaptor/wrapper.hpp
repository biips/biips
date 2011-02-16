//////////////////////////////////////////////////////////////////////////////
// cross_validation::estimator::adaptor::wrapper.hpp                        //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ESTIMATOR_ADAPTOR_WRAPPER_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ESTIMATOR_ADAPTOR_WRAPPER_HPP_ER_2009

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace estimator{
namespace adaptor{

    template<typename A>
    struct wrapper{
    
        wrapper(A& a):a_(a){}
    
        A& reference()const{ return this->a_; }
        const A& const_reference()const{ return this->a_; }
        
        private:
        wrapper();
        A& a_;
    
    };

}// adaptor
}// estimator
}// cross_validation
}// detail
}// statistics
}// boost

#endif