///////////////////////////////////////////////////////////////////////////////
// ars::point.hpp                                                            //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_POINT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_POINT_HPP_ER_2009
#include <ostream>
#include <boost/ars/constant.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{

// Given an unnormalized density f, this class is a representation for (x,y,dy), 
// where y = log f(x).
template <typename T>
class point{
    typedef constant<T> const_;

    public:
    point(): x_(const_::zero_),y_(const_::zero_),dy_(const_::zero_){}
    point(const T& x,const T& y,const T& dy)
    : x_(x),y_(y),dy_(dy){}

    const T& x()const{ return x_; }
    const T& y()const{ return y_; }
    const T& dy()const{ return dy_; }

    private:
    //abscissa, ordinate, derivative of the log density
    T x_, y_, dy_;
};

template<typename T>
void dump(const point<T>& p,T& x,T& y,T& dy){
    x = p.x();
    y = p.y();
    dy = p.dy();
}

template <typename T>
bool operator<(
    const point<T> &a,
    const point<T> &b
){
    return (a.x() < b.x());
}

template <typename T>
std::ostream&
operator<<(std::ostream &out, const point<T>& p)
{
    out << '(' << p.x() << ',' << p.y() << ',' << p.dy() << ')';
    return out;
}

template<typename T,typename F>
point<T>
create_point(
    const T& x,
    const F& f
){
    typedef point<T> result_t;
    T y, dy;
    f(x,y,dy);
    return result_t(x,y,dy);
}

template<typename T>
T tangent(const point<T>& a, const T& x){
    return a.y() + a.dy() * (x-a.x());
}

template<typename T>
bool is_non_increasing_dy(const point<T>&a,const point<T>& b){
    return !(a.dy()<b.dy());
}

template<typename T>
bool is_concave(const point<T>&a,const point<T>& b){
    T t_b = tangent(a,b.x());
    T t_a = tangent(b,a.x());
    return !( (t_b < b.y()) || (t_a < a.y()) );
}

template<typename T>
T linearly_interpolate(
    const point<T>& a,
    const point<T>& b,
    const T& x
){
    T slope =  (b.y() - a.y()) / (b.x() - a.x());
    return a.y() + (x-a.x()) * slope;
}

}// ars
}// detail
}// statistics
}// boost

#endif // BOOST_STATISTICS_DETAIL_ARS_POINT_HPP_ER_2009
