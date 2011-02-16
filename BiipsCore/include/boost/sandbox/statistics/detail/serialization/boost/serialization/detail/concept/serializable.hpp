//////////////////////////////////////////////////////////////////////////////
// serialization::detail::concept::serializable.hpp         				//
//                                                                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_SERIALIZATION_DETAIL_CONCEPT_SERIALIZABLE_HPP_ER_2010
#define BOOST_SERIALIZATION_DETAIL_CONCEPT_SERIALIZABLE_HPP_ER_2010
#include <boost/concept_check.hpp>
#include <boost/serialization/detail/concept/saveable.hpp>
#include <boost/serialization/detail/concept/loadable.hpp>

namespace boost{
namespace serialization{
namespace detail{
namespace concept{

    template<typename T>
    class Serializable : 
    	Saveable<T>,
        Loadable<T>
    {};
    
}// concept
}// detail
}// serialization
}// boost

#endif

