//////////////////////////////////////////////////////////////////////////////
// monomials_horner::multi_indexes.hpp                                      //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MONOMIALS_HORNER_MULTI_INDEXES_HPP_ER_2009
#define BOOST_MONOMIALS_HORNER_MULTI_INDEXES_HPP_ER_2009
#include <vector>
#include <functional>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/assert.hpp>
#include <boost/view/rows_iterator.hpp>
#include <boost/monomials_horner/monomials_properties.hpp>

namespace boost{
namespace monomials_horner{

    // Example of multi indexes for N = 3, degree =2
    // >(0,0,0)
    // >(1,0,0) >(0,1,0) >(0,0,1)
    // >(2,0,0) (1,1,0) (1,0,1) >(0,2,0) (0,1,1) >(0,0,2)
    // Note that
    // {(2,0,0) (1,1,0) (1,0,1)} = {(1,0,0) (0,1,0) (0,0,1)} + (1,0,0)
    // {(0,2,0) (0,1,1)} = {(0,1,0) (0,0,1)} + (0,1,0) etc.

    // N is the dimension of the multivariate monomial
    template<unsigned N>
    class multi_indexes{
      public:
        typedef std::vector<unsigned>                   storage_type;
        typedef typename storage_type::iterator         base_iter_type;
        typedef typename storage_type::size_type        size_type;
        typedef storage_type::value_type                base_value_type;

//        typedef matrix_view::row_iterator<base_iter_type>  iter_type;
        typedef view::detail::rows_iterator<base_iter_type>  iter_type;
        typedef std::vector<iter_type>                  iters_type;
        typedef typename iter_type::value_type          value_type;
        typedef iterator_range<iter_type>               iter_range_type;
        typedef typename iters_type::iterator           iters_iter_type;
        typedef typename iter_type::difference_type     iter_diff_type;
        typedef std::vector<iter_diff_type>             iters_diff_type;
        typedef typename iters_diff_type::iterator      iters_diff_iter_type;

        static iter_range_type get(unsigned int degree){
            static unsigned max_degree = 0;
            static storage_type storage(N,(base_value_type)(0));
            static iters_diff_type read_bs_dists(N,0);

// // keeping read_bs rather than read_bs_dists causes
// // error 'gliblc detected' at runtime
//            static iters_type   read_bs(
//                N,
//                matrix_view::make_row_iterator(
//                    begin(storage),
//                    N
//            ));

            //e.g. N=3: (1,0,0, 0,1,0, 0,0,1)
            static storage_type variables = variables_init();

            if(degree>max_degree){
                unsigned i = max_degree+1;
                unsigned n = degree+1;
                size_type sz_old = size(storage)/N;
                size_type sz_new
                    = properties::number_degree_less_than(degree,N);
                storage.resize(sz_new*N);
//                iter_type write_b = matrix_view::make_row_iterator(
//                    begin(storage),
//                    N
//                );
                iter_type write_b = view::detail::make_rows_iterator(
                    begin(storage),
                    end(storage),
                    N
                );
                std::advance(write_b,sz_old);

                iters_type read_bs;

//old implem, do not uncomment
//                iters_diff_iter_type b = begin(read_bs_dists);
//                iters_diff_iter_type e = end(read_bs_dists);
//                for(iters_diff_iter_type i = b; i<e; i++){
//                    iter_type it(begin(storage),N);
//                    std::advance(it,*i);
//                    read_bs.push_back(it);
//                }

                transform(
                    begin(read_bs_dists),
                    end(read_bs_dists),
                    back_inserter(read_bs),
                    unary_op(
//                        matrix_view::make_row_iterator(
//                            begin(storage),
//                            N
//                        )
                        view::detail::make_rows_iterator(
                            begin(storage),
                            end(storage),
                            N
                        )
                    )
                );

                while(i<n){
                    BOOST_ASSERT(size(read_bs_dists)==N);
                    BOOST_ASSERT(std::distance(
//                        matrix_view::make_row_iterator(
//                            begin(variables),
//                            N
//                        ),
//                        matrix_view::make_end_row_iterator(
//                                begin(variables),
//                                end(variables),
//                                N
//                        )

                        view::detail::make_rows_iterator(
                            begin(variables),
                            end(variables),
                            N
                        ),
                        view::detail::make_end_rows_iterator(
                                begin(variables),
                                end(variables),
                                N
                        )
                    )==N);

                    //(***) bug
                    write_b = for_each(
                        make_zip_iterator(
                            make_tuple(
                                begin(read_bs),
//                                matrix_view::make_row_iterator(
//                                    begin(variables),
//                                    N
//                                )
                                view::detail::make_rows_iterator(
                                    begin(variables),
                                    end(variables),
                                    N
                                )
                            )
                        ),
                        make_zip_iterator(
                            make_tuple(
                                end(read_bs),
//                                matrix_view::make_end_row_iterator(
//                                    begin(variables),
//                                    end(variables),
//                                    N
//                                )
                                view::detail::make_end_rows_iterator(
                                    begin(variables),
                                    end(variables),
                                    N
                                )
                            )
                        ),
                        zip_func(write_b)
                    ).write_b;
                    ++i;
                }
                read_bs_dists.clear();
                std::transform(
                    begin(read_bs),
                    end(read_bs),
                    back_inserter(read_bs_dists),
                    bind(
                        std::distance<iter_type>,
//                        matrix_view::make_row_iterator(
//                            begin(storage),
//                            N
//                        ),
                        view::detail::make_rows_iterator(
                            begin(storage),
                            end(storage),
                            N
                        ),
                        _1
                    )
                );
                max_degree = degree;
            }//if(degree>max_degree)

            size_type dist
                = properties::number_degree_less_than(degree,N);
            iter_type b(begin(storage),end(storage),N);
            iter_type e = b;
            std::advance(e,dist);
            iter_range_type(b,e);
            return iter_range_type(b,e);
        }//get(unsigned degree)

      private:
        typedef monomials_properties<> properties;
        multi_indexes(); //do not implement

        static storage_type variables_init(){
            storage_type variables(N*N,0);
            for(unsigned i=0; i<N; i++){
                variables[i*N+i]=1;
            }
            return variables;
        }

        struct unary_op{
            iter_type b;
            unary_op(iter_type b_):b(b_){}
            unary_op(const unary_op& that):b(that.b){}
            unary_op& operator=(const unary_op& that){
                if(&that!=this){b=that.b;}
                return *this;
            }
            typedef iter_type                   result_type;
            typedef const iter_diff_type&       argument_type;
            result_type operator()(argument_type diff)const{
                result_type it = b;
                std::advance(it,diff);
                return it;
            }
        };

        struct zip_func
        {
            typedef const tuple<
                iter_type&,const value_type&>&                  argument_type;
            typedef void                                        result_type;

            iter_type write_b;
            iter_type read_e;
            unsigned cnt;//TODO remove

            zip_func(iter_type write_b_)
            :write_b(write_b_),read_e(write_b),cnt(0){}

            zip_func(const zip_func& that)
            :write_b(that.write_b),
            read_e(that.read_e),
            cnt(that.cnt){}

            zip_func& operator=(const zip_func& that){
                if(&that!=this){
                    write_b = that.write_b;
                    read_e = that.read_e;
                    cnt = that.cnt;
                }
                return *this;
            }

            result_type operator()(argument_type t){
                value_type variable_ir = (t.template get<1>());
                iter_type read_b = (t.template get<0>());
                t.template get<0>() = write_b;
                write_b =
                    for_each(
                        read_b,
                        read_e,
                        unary_op(write_b,variable_ir)
                    ).write_b;
            }

            struct unary_op{
                unary_op(iter_type write_b_,value_type variable_ir)
                :write_b(write_b_),variable(variable_ir){}

                unary_op(const unary_op& that)
                :write_b(that.write_b),variable(that.variable){}

                unary_op& operator=(const unary_op& that)
                {
                    if(&that!=this){
                        write_b = that.write_b;
                        variable = that.variable;
                    }
                    return *this;
                }

                typedef typename iter_type::value_type     argument_type;
                typedef void                               result_type;
                result_type operator()(argument_type ir){

                    BOOST_ASSERT(size(*write_b)==size(ir));
                    BOOST_ASSERT(size(ir)==size(variable));
                    std::transform(
                        begin(ir),
                        end(ir),
                        begin(variable),
                        begin(*write_b),
                        bind(
                            std::plus<base_value_type>(),
                            _1,
                            _2
                        )
                    );
                    ++write_b;
                }
                iter_type   write_b;
                value_type  variable;
            };//unary_op
        };//zip_func
    };//multi_indexes

}// monomials_horner
}// boost


#endif
