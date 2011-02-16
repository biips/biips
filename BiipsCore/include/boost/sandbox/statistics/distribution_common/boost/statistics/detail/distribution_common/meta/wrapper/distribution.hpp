//////////////////////////////////////////////////////////////////////////////
// distribution::common::meta::wrapper_distribution.hpp                     //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_WRAPPER_DISTRIBUTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_WRAPPER_DISTRIBUTION_HPP_ER_2009
#include <boost/mpl/empty_base.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace meta{

    template<typename D,typename B = boost::mpl::empty_base>
    struct wrapper_distribution : B{
        typedef D distribution_type;
    };
    
}// meta
}// distribution
}// detail
}// statistics
}// boost

#endif
