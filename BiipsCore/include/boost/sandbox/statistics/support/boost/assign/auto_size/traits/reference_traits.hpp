//////////////////////////////////////////////////////////////////////////////
// assign::detail::reference_traits.hpp                                     //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_REFERENCE_TRAITS_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_REFERENCE_TRAITS_ER_2010_HPP
#include <utility>
#include <boost/mpl/and.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/or.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/assign/auto_size/traits/conversion_traits.hpp>

// Maps U = T& (or possibly U=T) to a type that is convertible to based on the
// conversion traits.

namespace boost{
namespace assign{
namespace detail{
namespace reference_traits{

    template<typename T>
    struct remove_cref : boost::remove_const<
        typename boost::remove_reference<T>::type
    >{};

    template<typename T>
    struct is_const : boost::is_const<
        typename boost::remove_reference<T>::type
    >{};

    template<bool use_ref,bool use_const,typename T>
    struct filter_ref_const{
       typedef typename reference_traits::remove_cref<T>::type value_;
       typedef typename boost::mpl::eval_if_c<
           use_const,
           boost::add_const<value_>,
           boost::mpl::identity<value_>
        >::type const_;
       typedef typename boost::mpl::eval_if_c<
           use_ref,
           boost::add_reference<const_>,
           boost::mpl::identity<const_>
        >::type type;
    };

    template<typename T>
    struct convert_to{
        typedef typename reference_traits::remove_cref<T>::type value_;
        typedef typename conversion_traits::convert_to<value_>::type to_;
        typedef typename boost::mpl::if_c<     
            boost::is_same<to_,value_>::value,
            T,
            to_    
        >::type type;
    };

}// reference_traits
}// detail
}// assign
}// boost

#endif
