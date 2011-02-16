//////////////////////////////////////////////////////////////////////////////
// assign::ref_list_of.hpp                                                  //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_AUTO_SIZE_REF_LIST_OF_ER_2010_HPP
#define BOOST_ASSIGN_AUTO_SIZE_REF_LIST_OF_ER_2010_HPP
#include <boost/assign/auto_size/detail/expr.hpp>

// Usage:
//    ref_list_of(a)(b)(c)  
//    ref_list_of<P>(a)(b)(c)  
//    cref_list_of(a)(b)(c)  
//    cref_list_of<P>(a)(b)(c)  

namespace boost{
namespace assign{

    // custom policy : 

    template<typename P,typename T>
    typename detail::auto_size::first_copy<const T,P>::type
    cref_list_of(const T& t){
        return detail::auto_size::first_copy<const T,P>::call(t);
    }

    template<typename P,typename T>
    typename detail::auto_size::first_copy<T,P>::type
    ref_list_of(T& t){
        return detail::auto_size::first_copy<T,P>::call(t);
    }

    // default policy :

    template<typename T>
    typename detail::auto_size::first_copy<const T>::type
    cref_list_of(const T& t){
        return detail::auto_size::first_copy<const T>::call(t);
    }

    template<typename T>
    typename detail::auto_size::first_copy<T>::type
    ref_list_of(T& t){
        return detail::auto_size::first_copy<T>::call(t);
    }

}// assign
}// boost

#endif
