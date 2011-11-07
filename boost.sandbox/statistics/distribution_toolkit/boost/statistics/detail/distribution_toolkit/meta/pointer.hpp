//////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::meta::pointer.hpp                                  //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_META_POINTER_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_META_POINTER_HPP_ER_2009
#include <boost/statistics/detail/distribution_toolkit/meta/signature.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

namespace meta{

    template<typename D>
    struct pointer{
        typedef typename signature<D>::type sig_;
        typedef sig_*                       type;

        template<template<typename> class F> // F in fun_wrap
        static type call(){ return F<D>::instance; }
    };

}// meta
}// distribution
}// toolkit

}// detail
}// statistics
}// boost

#endif
