//////////////////////////////////////////////////////////////////////////////
// detail::traits::add_cref.hpp                                             //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_TRAITS_ADD_CREF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_TRAITS_ADD_CREF_HPP_ER_2009
#include <boost/type_traits.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace traits{

    template<typename T>
    struct add_cref : add_reference< 
        typename add_const<T>::type
    >{};

}// traits
}// detail
}// statistics
}// boost

#endif
