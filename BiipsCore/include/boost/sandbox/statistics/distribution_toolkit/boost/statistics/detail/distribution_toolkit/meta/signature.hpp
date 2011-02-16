//////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::meta::signature.hpp                                //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_META_SIGNATURE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_META_SIGNATURE_HPP_ER_2009
#include <boost/statistics/detail/distribution_common/meta/value.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

namespace meta{

    // Signature for f(dist,x), f = pdf, cdf etc.
    template<typename D>
    struct signature{
        typedef typename distribution::meta::value<D>::type value_type;

        typedef value_type type(
            const D&,
            const value_type&
        );
    };

}// meta
}// distribution
}// toolkit

}// detail
}// statistics
}// boost

#endif
