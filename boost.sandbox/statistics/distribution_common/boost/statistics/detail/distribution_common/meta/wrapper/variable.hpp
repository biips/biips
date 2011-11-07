//////////////////////////////////////////////////////////////////////////////
// distribution::common::meta::wrapper_variable.hpp                         //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_WRAPPER_INPUT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_WRAPPER_INPUT_HPP_ER_2009
#include <boost/mpl/empty_base.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace meta{

    template<typename X,typename B = boost::mpl::empty_base>
    struct wrapper_variable : B{
        typedef X variable_type;
    };
    
}// meta
}// distribution
}// detail
}// statistics
}// boost

#endif
