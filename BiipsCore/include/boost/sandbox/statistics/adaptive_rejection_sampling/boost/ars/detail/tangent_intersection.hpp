///////////////////////////////////////////////////////////////////////////////
// ars::detail::tangent_intersection.hpp                                     //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_DETAIL_TANGENT_INTERSECTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_DETAIL_TANGENT_INTERSECTION_HPP_ER_2009
#include <boost/ars/constant.hpp>
#include <boost/ars/point.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{

        //    x1  x2  x3            point abscissae
        //   / \ / \ / \
        //  z0  z1  z2  z3 = zn     tangent abscissae
        //
        // z0 = x_min
        // zn = x_max
        // Let tang[i](x) = y[i] + dy[i](x-x[i])
        // z[i] solves : tang[i+1](z[i]) == tang[i](z[i])
        // t[i] = tang[i](z[i])

template<typename T>
struct tangent_intersection{
    typedef constant<T> const_;

    tangent_intersection()
    :z_(const_::zero_),
    t_(const_::zero_),
    cum_sum_(const_::zero_){}

    tangent_intersection(const point<T>& a,const point<T>& b){
        T eps = exp(const_::lmin_); //TODO numeric_smallest()?
        if(fabs(a.y()-b.y()) <= eps){
            z_ = (a.x() + b.x()) / const_::two_;
            t_ = (a.y() + b.y()) / const_::two_;
        }else{
            T ddy = b.dy() - a.dy();
            // For greater precision :
            if(fabs(a.dy())<fabs(b.dy())){
                z_ = b.x() + (a.y() - b.y() + a.dy() * (b.x() - a.x()))/ddy;
                t_ = a.y() + a.dy() * (z_ - a.x());
            }else{
                z_ = a.x() + (a.y() - b.y() + b.dy() * (b.x() - a.x()))/ddy;
                t_ = b.y() +  b.dy() * (z_ - b.x());
            }
        }
    }

    tangent_intersection(T z,T t)
    :z_(z), t_(t),cum_sum_(const_::zero_){}

    const T& z()const{ return z_; }
    const T& t()const{ return t_; }

    private:
    T z_; //abscissa
    T t_; //ordinate
    public:
    T cum_sum_; //area under exp tangent up to z_
};

template <typename T>
std::ostream&
operator<<(std::ostream& out, const tangent_intersection<T> &ti)
{
    out << '(' << ti.z() << ',' << ti.t() << ',' << ti.cum_sum_ << ')';
    return out;
}

template <typename T>
bool operator<(
    const tangent_intersection<T> &a,
    const tangent_intersection<T> &b
){
    return (a.cum_sum_ < b.cum_sum_);
}

}// ars
}// detail
}// statistics
}// boost

#endif
