//////////////////////////////////////////////////////////////////////////////
// assign::detail::has_copy_semantics.hpp                                   //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_HAS_COPY_SEMANTICS_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_HAS_COPY_SEMANTICS_ER_2010_HPP
#include <boost/mpl/bool.hpp>

namespace boost{
namespace assign_detail{
    template< class T > struct assign_reference;
}
namespace assign{
namespace detail{

    template<typename T> struct has_copy_semantics{};
    
    template< class T > struct assign_reference_copy;
    template< class T > struct assign_reference_rebind;
    
    template<typename T>
    struct has_copy_semantics<
    	assign_detail::assign_reference<T> 
    > : boost::mpl::bool_<false>{};

    template<typename T>
    struct has_copy_semantics<
    	assign_reference_copy<T> 
    > : boost::mpl::bool_<true>{};

    template<typename T>
    struct has_copy_semantics<
    	assign_reference_rebind<T> 
    > : boost::mpl::bool_<false>{};
    
}// detail
}// assign
}// boost

#endif

