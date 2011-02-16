//////////////////////////////////////////////////////////////////////////////
// monomials_horner::monomials_properties.hpp                               //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MONOMIALS_HORNER_MONOMIALS_PROPERTIES_HPP_ER_2009
#define BOOST_MONOMIALS_HORNER_MONOMIALS_PROPERTIES_HPP_ER_2009
#include <boost/math/special_functions/binomial.hpp>

namespace boost{
namespace monomials_horner{

    template<typename T = double>
    struct monomials_properties{
        static unsigned long number_degree_less_than(
            unsigned the_degree,unsigned dim){
                return static_cast<unsigned long>(math::binomial_coefficient<T>(
                    the_degree+dim,dim));
        }
    };

}// monomials_horner
}// boost


#endif
