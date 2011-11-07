///////////////////////////////////////////////////////////////////////////////
// sequence_to_unpack_variadic.hpp                                           //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MPL_DETAIL_SEQUENCE_TO_UNPACK_VARIADIC_HPP_ER_2010
#define BOOST_MPL_DETAIL_SEQUENCE_TO_UNPACK_VARIADIC_HPP_ER_2010

#include <boost/mpl/apply.hpp>
#include <boost/mpl/unpack_args.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>

// Usage: Let 0<=i2<n2
//  to< mpl::vector<T1,...,Tn1,U1,...Ui2> >::type = from<T1,..,Tn1,U1,...,Ui2>

#define BOOST_DETAIL_MPL_SEQUENCE_TO_UNPACK_VARIADIC(from,to,n1,n2,default)      \
template<typename Seq>                                                           \
struct to{                                                                       \
                                                                                 \
    struct meta{                                                                 \
        template<                                                                \
            BOOST_PP_ENUM_PARAMS(n1, typename T)                                 \
            BOOST_PP_COMMA_IF(n1)                                                \
            BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(n2, typename U, default)         \
        >                                                                        \
        struct apply{                                                            \
            typedef from<                                                        \
                BOOST_PP_ENUM_PARAMS(n1, T)                                      \
                BOOST_PP_COMMA_IF(n1)                                            \
                BOOST_PP_ENUM_PARAMS(n2, U)                                      \
            > type;                                                              \
        };                                                                       \
    };                                                                           \
                                                                                 \
    typedef typename boost::mpl::apply<                                          \
        boost::mpl::unpack_args<typename to::meta>,                              \
        Seq                                                                      \
    >::type type;                                                                \
};                                                                               \
/**/

#endif
