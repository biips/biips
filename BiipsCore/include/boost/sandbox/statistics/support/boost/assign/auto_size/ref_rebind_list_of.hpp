//////////////////////////////////////////////////////////////////////////////
// assign::ref_rebind_list_of.hpp                                           //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_AUTO_SIZE_REF_REBIND_LIST_OF_ER_2010_HPP
#define BOOST_ASSIGN_AUTO_SIZE_REF_REBIND_LIST_OF_ER_2010_HPP
#include <boost/assign/auto_size/detail/expr.hpp>

// Returns a collection builder with rebind semantics

namespace boost{
namespace assign{

    // custom policy : 

    template<typename P,typename T>
    typename detail::auto_size::first_rebind<const T,P>::type
    cref_rebind_list_of(const T& t){
        return detail::auto_size::first_rebind<const T,P>::call(t);
    }

    template<typename P,typename T>
    typename detail::auto_size::first_rebind<T,P>::type
    ref_rebind_list_of(T& t){
        return detail::auto_size::first_rebind<T,P>::call(t);
    }

    // default policy : 

    template<typename T>
    typename detail::auto_size::first_rebind<const T>::type
    cref_rebind_list_of(const T& t){
        return detail::auto_size::first_rebind<const T>::call(t);
    }

    template<typename T>
    typename detail::auto_size::first_rebind<T>::type
    ref_rebind_list_of(T& t){
        return detail::auto_size::first_rebind<T>::call(t);
    }

}// assign
}// boost


#endif
