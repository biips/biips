//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::expr_size.hpp                                 //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_EXPR_SIZE_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_EXPR_SIZE_ER_2010_HPP
#include <boost/mpl/empty_base.hpp>
#include <boost/mpl/int.hpp>
#include <boost/assign/auto_size/detail/types.hpp>

namespace boost{
namespace assign{
namespace detail{
namespace auto_size{

    template<typename E>
    struct expr_size{};

    template<>
    struct expr_size<top_> : boost::mpl::int_<0>{};
    
}// auto_size
}// detail
}// assign
}// boost

#endif
