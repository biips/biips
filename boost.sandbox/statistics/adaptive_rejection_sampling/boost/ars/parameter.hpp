///////////////////////////////////////////////////////////////////////////////
// ars::parameter.hpp                                                        //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_PARAMETER_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_PARAMETER_HPP_ER_2009
#include <boost/math/tools/precision.hpp>
#include <boost/ars/constant.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{

// These parameters are used as defaults, but they can be changed at
// runtime with sampler's interface.
//
// max_data_count:
// The algorithm switches to standard rejection sampling once this
// number is reached. Complex distributions may require a higher number.
//
// max_log:
// The higher this number, the greater the precision, but it should not
// exceed const_::lmax_.
// A lower number may be needed to ensure finiteness of certain
// quantities (?) such as cum_sum.
template<typename T>
struct parameter{
    static unsigned max_data_count;
    static T max_log;
    static std::size_t reserve;
    static unsigned n_max_reject;
};
template<typename T> unsigned parameter<T>::max_data_count = 20;
template<typename T>
T parameter<T>::max_log = constant<T>::lmax_/static_cast<T>(max_data_count);
template<typename T> std::size_t parameter<T>::reserve = 5;
template<typename T> unsigned parameter<T>::n_max_reject = 100;

}// ars
}// detail
}// statistics
}// boost

#endif
