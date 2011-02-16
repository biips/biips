///////////////////////////////////////////////////////////////////////////////
// ars::constant.hpp                                                         //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_CONSTANT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_CONSTANT_HPP_ER_2009
#include <limits>
#include <iostream>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/math/tools/precision.hpp>
#include <boost/type_traits.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{

    template<typename T>
    struct constant{
        typedef numeric::converter<T,unsigned> conv_t;
        static T zero_;
        static T eps_;
        static T one_;
        static T two_;
        static T lmin_; //exp(lmin_)>eps
        static T lmax_; //exp(lmax_)<inf
        static T inf_;
        static T quiet_nan_;
        static T sig_nan_;
    };
    template<typename T>
    T constant<T>::zero_ = conv_t::convert(0);
    template<typename T>
    T constant<T>::eps_ = boost::math::tools::epsilon<T>();
    template<typename T>
    T constant<T>::one_ = conv_t::convert(1);
    template<typename T>
    T constant<T>::two_ = conv_t::convert(2);
    template<typename T>
    T constant<T>::lmin_ = math::tools::log_min_value<T>();
    template<typename T>
    T constant<T>::lmax_ = math::tools::log_max_value<T>();
    template<typename T>
    T constant<T>::inf_ = std::numeric_limits<T>::infinity();
    template<typename T>
    T constant<T>::quiet_nan_ = std::numeric_limits<T>::quiet_NaN();
    template<typename T>
    T constant<T>::sig_nan_ = std::numeric_limits<T>::signaling_NaN();

}// ars
}// detail
}// statistics
}// boost

#endif
