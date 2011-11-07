///////////////////////////////////////////////////////////////////////////////
// unpack_map.hpp                                                            //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_FUSION_CONTAINER_MAP_DETAIL_UNPACK_MAP_HPP_ER_2010
#define BOOST_FUSION_CONTAINER_MAP_DETAIL_UNPACK_MAP_HPP_ER_2010
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/void.hpp>
#include <boost/parameter/config.hpp>
#include <boost/mpl/detail/sequence_to_unpack_variadic.hpp>

namespace boost{
namespace fusion{
namespace detail{

    // typedef mpl::vector<fusion::pair<K1,D1>,...fusion::pair<K1,D1> > vec_;
    // typedef unpack_map<vec_>::type map_;
    BOOST_DETAIL_MPL_SEQUENCE_TO_UNPACK_VARIADIC(
        boost::fusion::map,
        unpack_map,
        0,
        BOOST_PARAMETER_MAX_ARITY, 
        boost::fusion::void_
    );

}// detail
}// parameter
}// boost

#endif
