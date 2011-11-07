/////////////////////////////////////////////////////////////////////////////////////
// kernel::bandwidth_selection::normal_distribution.hpp                            //
//                                                                                 //
//  (C) Copyright 2009 Erwann Rogard                                               //
//  Use, modification and distribution are subject to the                          //
//  Boost Software License, Version 1.0. (See accompanying file                    // 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)               //
/////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_BANDWIDTH_SELECTION_NORMAL_DISRTIBUTION_HPP_ER_2009           
#define BOOST_STATISTICS_DETAIL_KERNEL_BANDWIDTH_SELECTION_NORMAL_DISRTIBUTION_HPP_ER_2009
#include <cmath>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{
namespace bandwidth_selection{

// Finds the optimal bandwidth for density estimation (Rosenblatt-Parzen) 
// assuming
// 1) The data is a normal vector each coordinate having stddev sigma
// 2) The kernel is Gaussian with dimension M

template<unsigned M, typename T>
struct normal_distribution{
    typedef T value_type;
    static value_type rp_bandwidth(std::size_t n); // Assumes sigma = 1
    static value_type rp_bandwidth(value_type sigma,unsigned n);
    // For M == 1, bandwdith = sigma (3n/4)^(-1/5) = 1.06 sigma n^(-1/5)
};
template<unsigned M, typename T>
typename normal_distribution<M,T>::value_type
normal_distribution<M,T>::rp_bandwidth(std::size_t n){
    static const value_type e = -static_cast<T>(1)/static_cast<T>(M+4);
    static const value_type r = static_cast<T>(2+M)/ static_cast<T>(4);
    value_type a =  static_cast<T>(n) * r;
    return std::pow( a, e );
}

template<unsigned M, typename T>
typename normal_distribution<M,T>::value_type
normal_distribution<M,T>::rp_bandwidth(value_type sigma,unsigned n){
    return sigma * rp_bandwidth(n);
}

}// bandwidth_selection
}// kernel
}// detail
}// statistics
}// boost

#endif
