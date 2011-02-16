//////////////////////////////////////////////////////////////////////////////
// monomials_horner::policy::multi_degree.hpp                               //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MONOMIALS_HORNER_POLICY_MULTI_DEGREE_HPP_ER_2009
#define BOOST_MONOMIALS_HORNER_POLICY_MULTI_DEGREE_HPP_ER_2009
#include <functional>

namespace boost{
namespace monomials_horner{

    // (a,b,c) --> a + b + c (also called total degree)
    struct multi_degree : public std::plus<unsigned>{
        static result_type initial_value;
    };
    unsigned multi_degree:: initial_value = static_cast<unsigned>(0);

}// monomials_horner
}// boost

#endif