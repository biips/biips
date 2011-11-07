//////////////////////////////////////////////////////////////////////////////
// cross_validation::data::input_output_generator.hpp                       //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_DATA_INPUT_OUTPUT_GENERATOR_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_DATA_INPUT_OUTPUT_GENERATOR_HPP_ER_2009

#include <boost/statistics/detail/cross_validation/data/input_output.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace data{

    // Maps x to a data-type representing (x,f(x))
    //
    // Note: the library does not require a specific data-type; this one is
    // provided as a convenience
    //
    // private inheritance of F was abandonned after compilor error with 
    // F = function<R(A)>
    template<
        typename X,typename F,
        typename Kx = mpl::int_<1>,
        typename Ky = mpl::int_<2>
    >
    class input_output_generator {
        typedef meta_input_output<X,F,Kx,Ky> meta_;

        public:

        typedef meta_ meta_input_output;
        
        typedef typename meta_::type result_type;

        input_output_generator(){}

        input_output_generator(const F& f):f_(f){}
        input_output_generator(const input_output_generator& that)
            :f_(that.f_){}
        input_output_generator& 
        operator=(const input_output_generator& that)
        {
            if(&that!=this){
                this->f_ = that.f_;
            }
            return (*this);
        }
        
        template<typename X1>
        result_type operator()(const X1& x)const{
            return meta_::make(x,this->f_);
        }

        protected: 
        mutable F f_;
        
    };
    
}// data
}// cross_validation
}// detail
}// statistics
}// boost

#endif