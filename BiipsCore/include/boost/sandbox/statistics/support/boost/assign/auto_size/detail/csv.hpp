//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::csv.hpp                                       //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_DETAIL_CSV_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_DETAIL_CSV_ER_2010_HPP
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/assign/auto_size/detail/expr.hpp>
#include <boost/assign/auto_size/array/static.hpp>

// Whereas adjacent unary function calls is the usual way to create a collec-
// tion in Boost.Assign, this macro provides, as an alternative, functions that 
// are overloaded on the number of arguments. 
//
// Let n = BOOST_ASSIGN_CSV_SIZE and a1,...,an, objects of type T, Ref an alias 
// for BOOST_ASSIGN_CSV_ref, P0 = BOOST_ASSIGN_CSV_DEF_POLICY and r<U,N,P> an
// alias for BOOST_ASSIGN_CSV_RESULT(U,N,P) 
//
// Usage:
// BOOST_ASSIGN_CSV(fun) creates for i=1,...,n the following overloads:
// Call                             Result
//  fun<P>(a1,..,.ai)               r<T,i,P>
//  cfun<P>(a1,..,.ai)              r<const T,i,P>
//  fun(a1,..,.ai)                  r<T,i,P0>
//  cfun(a1,..,.ai)                 r<const T,i,P0>
//
// Note : It is critical that the collection builder auto_size::expr<> be used 
// only during construction of the result, not as part of the result itself. 
// Keep this in mind in designing a custom policy.

#ifndef BOOST_ASSIGN_CSV_SIZE
#define BOOST_ASSIGN_CSV_SIZE 20
#endif

#ifndef BOOST_ASSIGN_CSV_ref
#error
#endif
#define BOOST_ASSIGN_CSV_DEF_POLICY                                         \
    boost::assign::detail::auto_size::tag::array                            \
/**/
#define BOOST_ASSIGN_CSV_RESULT(U,N,P)                                      \
    typename boost::assign::detail::auto_size::csv_policy<P>::template      \
        apply<U,N,BOOST_ASSIGN_CSV_ref>::type                               \
/**/
#define BOOST_ASSIGN_CSV_RESULT_DEF_POLICY(U,N)                             \
    BOOST_ASSIGN_CSV_RESULT(U,N,BOOST_ASSIGN_CSV_DEF_POLICY)                \
/**/

#define BOOST_ASSIGN_CSV_ARG(z,n,arg) (BOOST_PP_CAT(arg,n))
#define BOOST_ASSIGN_CSV_TPL(arg)                                           \
     BOOST_PP_CAT(<,                                                        \
    	BOOST_PP_CAT(arg,>))                                                \
/**/

#define BOOST_ASSIGN_CSV_ITER_UNQUAL(F,T,U,N)                               \
namespace boost{                                                            \
namespace assign{                                                           \
    template<typename P,typename T>                                         \
    BOOST_ASSIGN_CSV_RESULT(U,N,P)                                          \
    F(BOOST_PP_ENUM_PARAMS(N, U& _))                                        \
    {                                                                       \
        return boost::assign::detail::auto_size::make_first_expr_no_policy< \
                BOOST_ASSIGN_CSV_ref,T                                      \
            > BOOST_PP_REPEAT(N,BOOST_ASSIGN_CSV_ARG,_);                    \
    }                                                                       \
    template<typename T>                                                    \
    BOOST_ASSIGN_CSV_RESULT_DEF_POLICY(U,N)                                 \
    F(BOOST_PP_ENUM_PARAMS(N, U& _))                                        \
    {                                                                       \
        return boost::assign::detail::auto_size::make_first_expr_no_policy< \
                BOOST_ASSIGN_CSV_ref,T                                      \
            > BOOST_PP_REPEAT(N,BOOST_ASSIGN_CSV_ARG,_);                    \
    }                                                                       \
}                                                                           \
}                                                                           \
/**/

#define BOOST_ASSIGN_CSV_ITER(F,N)                                          \
    BOOST_ASSIGN_CSV_ITER_UNQUAL(F,T,T,N)                                   \
    BOOST_ASSIGN_CSV_ITER_UNQUAL(BOOST_PP_CAT(c,F),T,const T,N)             \
/**/

// overloads begin at n = 1
#define BOOST_ASSIGN_CSV_SHIFTED_ITER(z,n,F)                                \
    BOOST_ASSIGN_CSV_ITER(F,BOOST_PP_ADD(n,1))                              \
/**/

#define BOOST_ASSIGN_CSV_REPEAT(fun,N)                                      \
    BOOST_PP_REPEAT(N,BOOST_ASSIGN_CSV_SHIFTED_ITER,fun)                    \
/**/

#define BOOST_ASSIGN_CSV(fun)                                               \
    BOOST_ASSIGN_CSV_REPEAT(fun,BOOST_ASSIGN_CSV_SIZE)                      \
/**/

#endif
