///////////////////////////////////////////////////////////////////////////////
// ars::function::signature.hpp                                               //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_FUNCTION_SIGNATURE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_FUNCTION_SIGNATURE_HPP_ER_2009
#include <boost/mpl/identity.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{
namespace function{

    // This is the function signature required by the ars sampler.
    //
    // Given input x, fun(x, y, dy) writes the log unnormalized pdf and its
    // derivative at x to y and dy, respectively.
    template<typename T>
    struct signature : mpl::identity<
        void(const T&,T&,T&)
    >{};

}// function
}// ars
}// detail
}// statistics
}// boost

#endif