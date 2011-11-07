//////////////////////////////////////////////////////////////////////////////
// assign::detail::conversion_traits.hpp                                    //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_CONVERSION_TRAITS_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_CONVERSION_TRAITS_ER_2010_HPP
#include <boost/mpl/identity.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/assign/auto_size/traits/inner_value_traits.hpp>

namespace boost{
namespace assign{
namespace detail{

// A type T is mapped to a convertible type U in two steps:
//     1) Tag = tag_of<T>::type
//     2) U = meta::convert_to<Tag>::apply<T>::type
// 
// Groups defined in this file: 
//          Tag                         U                   
//   itself (the default)               T
//   reference_to_inner_value     inner_value<T>::type&

namespace conversion_traits{

    namespace tag{ 
        struct itself{ typedef itself type; }; 
        struct reference_to_inner_value{ 
            typedef reference_to_inner_value type; 
        };
        
        // Add as needed
    }

    // Specialize as needed
    template<typename T> struct tag_of : tag::itself{};

    namespace meta{ 
        template<typename T> struct identity : boost::mpl::identity<T>{};
    
        template<typename Tag> struct convert_to{
            template<typename T> struct apply{};
        }; 

        template<> 
        struct convert_to<conversion_traits::tag::itself>{
            template<typename T> struct apply : identity<T>{};            
        };

        template<> 
        struct convert_to<conversion_traits::tag::reference_to_inner_value>{
            template<typename T>
            struct apply : boost::add_reference<
                typename detail::inner_value_traits::inner_value_of<T>::type
            >{};
        };
        // Specialize further as needed
    }// meta

    template<typename T>
    struct convert_to
        : meta::convert_to<
           typename conversion_traits::tag_of<T>::type
        >::template apply<T>{};

}// conversion_traits
}// detail
}// assign
}// boost

#endif
