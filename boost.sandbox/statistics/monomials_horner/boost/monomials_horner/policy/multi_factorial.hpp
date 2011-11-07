//////////////////////////////////////////////////////////////////////////////
// monomials_horner::policy::multi_factorial.hpp                            //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MONOMIALS_HORNER_POLICY_MULTI_FACTORIAL_HPP_ER_2009
#define BOOST_MONOMIALS_HORNER_POLICY_MULTI_FACTORIAL_HPP_ER_2009
#include <boost/math/special_functions/factorials.hpp>

namespace boost{
namespace monomials_horner{

    // (a,b,c) --> a! b! c!
    struct multi_factorial{
        typedef unsigned                      result_type;
        typedef unsigned                      first_argument_type;
        typedef unsigned                      second_argument_type;
        multi_factorial(){}
        result_type operator()(
            first_argument_type a1,
            second_argument_type a2
        )const{
            return a1 * static_cast<result_type>(math::factorial<double>(a2));
        }
        static result_type initial_value;
    };
    multi_factorial::result_type multi_factorial:: initial_value
        = static_cast<result_type>(1);

}// monomials_horner
}// boost

#endif