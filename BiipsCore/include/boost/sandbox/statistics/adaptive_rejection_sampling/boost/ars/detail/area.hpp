///////////////////////////////////////////////////////////////////////////////
// ars::detail::area.hpp                                                     //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_DETAIL_AREA_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_DETAIL_AREA_HPP_ER_2009
#include <stdexcept>
#include <boost/ars/detail/data.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{

// Area of the exponential tangent between two
// 2 intersections (one of which is possibly -inf or inf).

template<typename T>
T area_flat_segment(
    const tangent_intersection<T>& a,
    const tangent_intersection<T>& b,
    const T& offset
){
    typedef constant<T> const_;
    T mid = (a.t() + b.t()) / const_::two_;
    return (b.z() - a.z()) * exp(mid - offset);
}

template<typename T>
T area_left_tail(
    const data<T>& b,
    const T& offset
){
    return exp( b.t() - offset) / b.dy();
}
template<typename T>
T area_right_tail(
    const tangent_intersection<T>& a,
    const point<T>& b,
    const T& offset
){
    return - exp( a.t() - offset) / b.dy();
}

template<typename T>
T area_segment(
    const tangent_intersection<T>& a,
    const data<T>& b,
    const T& offset
){
    typedef constant<T> const_;
    // Note that b.dy() (a.z_-b.z_) == a.t_ - b.t_
    T dt  = a.t() - b.t();
    return exp( b.t() - offset) * (const_::one_ - exp(dt)) / b.dy();
}

template<typename T>
T area_segment_safeguarded(
    const tangent_intersection<T>& a,
    const data<T>& b,
    const T& offset
)
{
    typedef constant<T> const_;
    T area;
    if(fabs(b.dy())<const_::lmin_){
        area = area_flat_segment(a,b,offset);
    }else{
        if(a.t()-b.t()>const_::lmax_){
            // exp(a.t_-iter_.t_) = inf,
            // so 1) not computable and 2) b negligible
            // This alternative is computable because (in principle)
            // exp(a.t_- offset) <inf
            area = area_right_tail(a,b,offset);
        }else{
            area = area_segment(a,b,offset);
        }
    }
    return area;
}


}// ars
}// detail
}// statistics
}// boost

#endif
