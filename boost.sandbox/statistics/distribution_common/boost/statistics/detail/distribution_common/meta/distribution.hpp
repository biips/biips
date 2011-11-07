//////////////////////////////////////////////////////////////////////////////
// distribution::common::meta::distribution.hpp                             //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_DISTRIBUTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_DISTRIBUTION_HPP_ER_2009
#include <boost/mpl/identity.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace meta{

    template<typename D>
    struct distribution : boost::mpl::identity<
        typename D::distribution_type
    >{};
    
}// meta
}// distribution
}// detail
}// statistics
}// boost

#endif
