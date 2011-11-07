///////////////////////////////////////////////////////////////////////////////
// unpack_parameters.hpp                                                     //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ACCUMULATORS_DETAIL_UNPACK_PARAMETERS_HPP_ER_2010
#define BOOST_ACCUMULATORS_DETAIL_UNPACK_PARAMETERS_HPP_ER_2010
#include <boost/parameter/parameters.hpp>
#include <boost/mpl/void.hpp>
#include <boost/mpl/detail/sequence_to_unpack_variadic.hpp>

namespace boost{
namespace parameter{
namespace detail{

    BOOST_DETAIL_MPL_SEQUENCE_TO_UNPACK_VARIADIC(
        boost::parameter::parameters,
        unpack_parameters,
        1,
        4,//BOOST_PARAMETER_MAX_ARITY-1,
        boost::mpl::void_
    );

}// detail
}// parameter
}// boost

#endif
