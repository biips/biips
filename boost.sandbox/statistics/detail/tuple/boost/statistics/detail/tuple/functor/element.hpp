///////////////////////////////////////////////////////////////////////////////
// statistcs::detail::tuple::functor::tuple_element.hpp                      //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_TUPLE_FUNCTOR_TUPLE_ELEMENT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_TUPLE_FUNCTOR_TUPLE_ELEMENT_HPP_ER_2009
#include <boost/tuple/tuple.hpp>
#include <boost/type_traits.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace tuple{
namespace functor{


    // maps a tuple to its i^th element
    //
    // See http://lists.boost.org/boost-users/2005/07/12644.php
    template<unsigned i>
    struct element{

        template<typename S>
        struct result{};
    
        template<typename F,typename T>
        struct result<F(T&)>{
            typedef typename boost::tuples::element<i,T>::type type;
        };
        
        template<typename S>
        struct sig : result<S>{};

        template<typename T>
        typename result<element(T&)>::type
        operator()(T& t)const{
            return boost::get<i>(t);
        }

        template<typename T>
        typename result<element(const T&)>::type
        operator()(const T& t)const{
            return boost::get<i>(t);
        }


    };
    
}// functor
}// tuple
}// detail
}// statistics
}// boost

#endif