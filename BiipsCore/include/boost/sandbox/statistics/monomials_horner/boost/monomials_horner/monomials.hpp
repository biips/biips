//////////////////////////////////////////////////////////////////////////////
//  monomials_horner::monomials.hpp                                         //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MONOMIALS_HORNER_MONOMIALS_HPP_ER_2009
#define BOOST_MONOMIALS_HORNER_MONOMIALS_HPP_ER_2009
#include <vector>
#include <algorithm>
#include <iterator>
#include <boost/assert.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/monomials_horner/monomials_properties.hpp>

namespace boost{
namespace monomials_horner{

    // Background:    
    // \see http://mathworld.wolfram.com/Polynomial.html
    // A polynomial is a mathematical expression involving a sum of powers in 
    // one or more variables  multiplied by coefficients. A polynomial in one 
    // variable (i.e., a univariate polynomial) with constant coefficients is 
    // given by 
    //  a_nx^n+...+a_2x^2+a_1x+a_0. (1)
    // The individual summands with the coefficients (usually) included are 
    // called monomials (Becker and Weispfenning 1993, p. 191),
    // the products of the form x_1^(alpha_1)...x_n^(alpha_n)
    // in the multivariate case,
    // i.e., with the coefficients omitted, are called terms
    // (Becker and Weispfenning 1993, p. 188).
    // The highest power  in a univariate polynomial is called its order,
    // or sometimes its degree


    // Example :
    // variables = (a,b,c)
    //
    // 1
    // >a >b >c
    // >a^2 ab ac >b^2 bc >c^2
    // >a^3 a^2b a^2 c ab^2 abc ac^2 >b^3 b^2c bc^2 > c^3
    //
    // each > above marks the begining of a "read range"
    // the kth write range on the nth line is obtained by
    // multiplying the kth read range on the (n-1)th line
    // by the kth variable in (a,b,c)
    // For example, (b^3, b^2c, bc^2) = b*(b^2, bc, c^2)

    template<typename Cont = std::vector<double> >
    class monomials{
        typedef typename Cont::size_type                size_type;
        typedef typename range_iterator<Cont>::type     it_;
        typedef std::vector<it_>                        it_s;
        typedef monomials_properties<>                  properties;
    public:
        typedef typename range_value<Cont>::type        value_type;
        typedef const Cont&                             result_type;
        monomials(){}
        monomials(monomials& that)
        :degree_(that.degree_),terms_(that.terms_){}
        monomials& operator=(const monomials& that){
            if(&that!=this){
                degree_ = that.degree_;
                terms_ = that.terms_;
            }
            return *this;
        }

        //degree (also called total degree) is |alpha|=alpha_0+...+alpha_{n-1}
        unsigned degree()const{return degree_;}
        result_type operator()()const{return terms_;}

        template<typename R>
        void operator()(const R& variables,unsigned the_degree){
            typedef std::vector<it_> it_s;
            static it_s read_bs;
            {
                size_type sz = properties::number_degree_less_than(
                    the_degree,(unsigned)(size(variables)));
                terms_.resize(sz);
            }
            it_ write_b = begin(terms_);

            *write_b++ = 1;
            read_bs.clear();
            std::fill_n(
                back_inserter(read_bs),
                size(variables),
                begin(terms_)
            );

            unsigned i = 1;
            unsigned n = the_degree + 1;
            while(i<n){
                BOOST_ASSERT(size(read_bs)==size(variables));
                write_b = for_each(//e=
                    make_zip_iterator(
                        make_tuple(
                            begin(read_bs),
                            begin(variables)
                        )
                    ),
                    make_zip_iterator(
                        make_tuple(
                            end(read_bs),
                            end(variables)
                        )
                    ),
                    zip_func<R>(write_b)
                ).write_b;

                ++i;
            }
            degree_ = the_degree;
        }

    private:
        // keeping terms_ inside spares the client the need to clear it
        // or keeping track of the number of terms.
        // using the same Cont for improved memory management
        unsigned degree_;
        Cont terms_;

        template<typename R>
        struct zip_func
        {
            typedef typename range_iterator<R>::type              it_1;
            typedef typename it_1::value_type                    value1_type;
            typedef const tuple<it_&,const value1_type&>& argument_type;
            typedef void                                          result_type;

            zip_func(it_ write_b_)
            :write_b(write_b_),read_e(write_b){}

            zip_func(const zip_func& that)
            :write_b(that.write_b),read_e(that.read_e){}

            zip_func& operator=(const zip_func& that){
                if(&that!=this){
                    write_b = that.write_b;
                    read_e = that.read_e;
                }
                return *this;
            }

            result_type operator()(argument_type t){
                typename it_1::value_type x = (t.template get<1>());

                it_ read_b = t.get<0>();
                t.template get<0>() = write_b;

                write_b =
                    transform(
                        read_b,
                        read_e,
                        write_b,
                        bind(
                            std::multiplies<value_type>(),
                            _1,
                            x
                        )
                    );
            }

            it_ write_b;
            it_ read_e;

        };
    };

}// monomials_horner
}// boost

#endif
