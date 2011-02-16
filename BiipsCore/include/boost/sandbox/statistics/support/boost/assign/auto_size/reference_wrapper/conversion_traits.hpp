//////////////////////////////////////////////////////////////////////////////
// assign::detail::reference_wrapper::conversion_traits.hpp                 //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_REFERENCE_WRAPPER_CONVERSION_TRAITS_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_REFERENCE_WRAPPER_CONVERSION_TRAITS_ER_2010_HPP
#include <boost/assign/auto_size/reference_wrapper/inner_value_traits.hpp>
#include <boost/assign/auto_size/traits/conversion_traits.hpp>

namespace boost{
namespace assign{
namespace detail{

// fwd declare
template<typename T> struct assign_reference_copy;
template<typename T> struct assign_reference_rebind;

namespace conversion_traits{

    template<typename T> 
    struct tag_of< detail::assign_reference_copy<T> > 
        : tag::reference_to_inner_value{};

    template<typename T> 
    struct tag_of< detail::assign_reference_rebind<T> > 
        : tag::reference_to_inner_value{};

}// conversion_traits
}// detail
}// assign
}// boost

#endif
