//////////////////////////////////////////////////////////////////////////////
// monomials_horner::policy::power2divfact.hpp                              //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MONOMIALS_HORNER_POLICY_MULTI_POWER2DIVFACT_HPP_ER_2009
#define BOOST_MONOMIALS_HORNER_POLICY_MULTI_POWER2DIVFACT_HPP_ER_2009
#include <boost/math/special_functions/factorials.hpp>

namespace boost{
namespace monomials_horner{

    // (a,b,c) --> 2^{a+b+c}/(a! b! c!)
    struct multi_power2divfact{
        typedef double                      result_type;
        typedef double                      first_argument_type;
        typedef unsigned                    second_argument_type;
        multi_power2divfact(){}
        result_type operator()(
            first_argument_type a1,
            second_argument_type a2
        )const{
            result_type d = std::pow(2.0,static_cast<result_type>(a2));
            result_type f = math::factorial<result_type>(a2);
            result_type r = d/f;
            return a1 * static_cast<result_type>(r);
        }
        static result_type initial_value;
    };
    multi_power2divfact::result_type multi_power2divfact::initial_value
        = static_cast<result_type>(1);

}// monomials_horner
}// boost

#endif
