//////////////////////////////////////////////////////////////////////////////
// assign::detail::range::result_of_chain.hpp                               //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_RANGE_RESULT_OF_CHAIN_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_RANGE_RESULT_OF_CHAIN_ER_2010_HPP
#include <boost/typeof/typeof.hpp>
#include <boost/range/chain.hpp>

namespace boost{
namespace assign{
namespace detail{

namespace result_of{

    template<typename R1,typename R2>
    struct chain{
        static R1 r1;
        static R2 r2;

        #ifdef BOOST_MSVC
        typedef BOOST_TYPEOF_NESTED_TYPEDEF_TPL(
            nested,
            boost::chain(r1,r2)
        );
        typedef typename nested::type type;
        #endif

        #ifndef BOOST_MSVC
         typedef BOOST_TYPEOF_TPL( boost::chain(r1,r2) ) type;
        #endif
    };

}

}// detail
}// assign
}// boost

#endif