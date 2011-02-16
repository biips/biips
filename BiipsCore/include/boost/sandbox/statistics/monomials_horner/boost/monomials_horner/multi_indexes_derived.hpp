//////////////////////////////////////////////////////////////////////////////
// monomials_horner::multi_indexes_derived.hpp                              //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MONOMIALS_HORNER_MULTI_INDEXES_DERIVED_HPP_ER_2009
#define BOOST_MONOMIALS_HORNER_MULTI_INDEXES_DERIVED_HPP_ER_2009
#include <numeric>
#include <cmath>
#include <boost/monomials_horner/multi_indexes.hpp>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/monomials_horner/policy/multi_factorial.hpp>
#include <boost/monomials_horner/policy/multi_degree.hpp>
#include <boost/monomials_horner/policy/multi_power2divfact.hpp>

namespace boost{
namespace monomials_horner{

    // Functions of multi_indexes
    // The particular function is controled by a policy:
    template<unsigned int N,typename Policy>
    class multi_indexes_derived{
      public:
        typedef typename Policy::result_type            value_type;
        typedef std::vector<value_type>                 storage_type;
        typedef typename storage_type::iterator         iter_type;
        typedef iterator_range<iter_type>               iter_range_type;
        typedef typename storage_type::size_type        size_type;

       static iter_range_type get(unsigned int degree){
            static unsigned max_degree = 0;
            static storage_type storage(1,Policy::initial_value);
            size_type dist
                = monomials_properties<>::number_degree_less_than(
                    degree,N);
            if(max_degree<degree){
                typedef multi_indexes<N>            mi_type;
                typedef
                    typename mi_type::iter_range_type      mi_iter_range_type;
                mi_iter_range_type ir = mi_type::get(degree);
                storage.clear();
                transform(
                    begin(ir),
                    end(ir),
                    back_inserter(storage),
                    unary_op()
                );
                max_degree = degree;
            }

            iter_type b(begin(storage));
            iter_type e(b); std::advance(e,dist);

            return iter_range_type(b,e);
        }
      private:
        multi_indexes_derived(); //do not implement

        typedef multi_indexes<N>  mi_type;
        typedef typename mi_type::iter_range_type       mi_iter_range_type;
        typedef typename mi_type::iter_type             mi_iter_type;
        typedef typename mi_type::value_type            mi_value_type;
        typedef typename mi_type::base_iter_type        mi_base_iter_type;
        typedef typename mi_type::base_value_type       mi_base_value_type;

        struct unary_op{
            typedef mi_value_type                       argument_type;
            typedef value_type                          result_type;
            unary_op(){}
            result_type operator()(argument_type ir){
                return accumulate(
                    begin(ir),
                    end(ir),
                    Policy::initial_value, 
                    Policy()
                );
            }
            iter_type write_begin;
        };
    };
    
}// monomials_horner
}// boost

#endif
