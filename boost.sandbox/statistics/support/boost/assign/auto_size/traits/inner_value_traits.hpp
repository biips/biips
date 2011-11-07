//////////////////////////////////////////////////////////////////////////////
// assign::detail::inner_value_traits.hpp                                   //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_INNER_VALUE_TRAITS_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_INNER_VALUE_TRAITS_ER_2010_HPP

namespace boost{
namespace assign{
namespace detail{
namespace inner_value_traits{

// A type T is mapped to a its inner_value, U, in two steps:
//     1) Tag = tag_of<T>
//     2) U = meta::inner_value<Tag>::apply<T>::type
// This file defines one group :
//          Tag               T            U       
//  nested_parameter         W<V>          V

    namespace tag{ 
        struct nested_parameter{ typedef nested_parameter type; }; 
    }// tag

    // Specialize as needed
    template<typename T> struct tag_of : tag::nested_parameter{};

    namespace meta{ 
    
        template<typename Tag> struct inner_value_of
        {
            template<typename T> struct apply{};
        }; 

        #ifndef BOOST_MSCV
        template<> 
        struct inner_value_of<
            inner_value_traits::tag::nested_parameter
        >{ template<typename T> struct apply{}; };
        #endif

        template<> 
        template<template<typename> class W,typename T>
        struct inner_value_of<inner_value_traits::tag::nested_parameter>
            ::apply< W<T> >{ typedef T type; };

        // Specialize further as needed
    }// meta

    template<typename T>
    struct inner_value_of
        : meta::inner_value_of<
            typename inner_value_traits::tag_of<T>::type>::template apply<T>{};

}// inner_value_traits

}// detail
}// assign
}// boost

#endif
