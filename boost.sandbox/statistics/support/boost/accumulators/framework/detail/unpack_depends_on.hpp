///////////////////////////////////////////////////////////////////////////////
// unpack_depends_on.hpp                                                     //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ACCUMULATORS_DETAIL_UNPACK_DEPENDS_ON_HPP_ER_2010
#define BOOST_ACCUMULATORS_DETAIL_UNPACK_DEPENDS_ON_HPP_ER_2010
#include <boost/mpl/aux_/na.hpp>
#include <boost/accumulators/accumulators_fwd.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/mpl/detail/sequence_to_unpack_variadic.hpp>

namespace boost{
namespace accumulators{
namespace detail{

    // typedef boost::mpl::vector<tag1,...,tagn> tags_;
    // typedef unpack_depends_on<tags_>::type depends_on_;
    // struct feature : depens_on_{};
    BOOST_DETAIL_MPL_SEQUENCE_TO_UNPACK_VARIADIC(
        boost::accumulators::depends_on,
        unpack_depends_on,
        0,
        BOOST_ACCUMULATORS_MAX_FEATURES,
        boost::mpl::na
    );

}// detail
}// accumulators
}// boost

#endif
