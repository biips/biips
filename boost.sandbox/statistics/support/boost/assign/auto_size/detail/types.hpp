//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::types.hpp                                     //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_TYPES_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_TYPES_ER_2010_HPP
#include <boost/mpl/void.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/assign/auto_size/array/tag.hpp>

namespace boost{
namespace assign{
namespace detail{
namespace auto_size{

    namespace tag{
        struct no_policy;
    }

    typedef boost::mpl::void_ top_;
    typedef tag::array default_policy;
    typedef tag::no_policy no_policy;
    typedef boost::mpl::bool_<false> false_;
    typedef boost::mpl::bool_<true> true_;
    
}// auto_size
}// detail
}// assign
}// boost

#endif
