///////////////////////////////////////////////////////////////////////////////
// distribution::common::meta::inherit_value.hpp                             //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_INHERIT_VALUE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_META_INHERIT_VALUE_HPP_ER_2009
#include <boost/mpl/assert.hpp>
#include <boost/mpl/not.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/empty_base.hpp>
#include <boost/statistics/detail/distribution_common/meta/value.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace meta{

    template<typename D,typename B = mpl::empty_base>
    struct inherit_value : B{
        BOOST_MPL_ASSERT((
            mpl::not_<
                is_reference<D>
            >
        ));

        typedef typename value<D>::type value_type;
        
    };

}// meta
}// common
}// detail
}// statistics
}// boost

#endif
