///////////////////////////////////////////////////////////////////////////////
// ars::detail::data.hpp                                                     //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_DETAIL_DATA_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_DETAIL_DATA_HPP_ER_2009
#include <boost/function.hpp>
#include <boost/math/tools/precision.hpp>
#include <boost/ars/constant.hpp>
#include <boost/ars/point.hpp>
#include <boost/ars/detail/tangent_intersection.hpp>
#include <boost/ars/error.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{

template<typename T>
struct data : point<T>, tangent_intersection<T>{
    typedef constant<T> const_;
    typedef point<T> super1;
    typedef tangent_intersection<T> super2;
    data():super1(),super2(){}
    data(const super1& p):super1(p),super2(){}
    data(const super1& p,const super2& ti):super1(p),super2(ti){}

};

// Warning:
// Since both point<T> and tangent<T> have operator <,
// use a const reference to the desired type for a and b


template <typename T>
std::ostream&
operator<<(std::ostream& out, const data<T> &d){
    typedef point<T> point_t;
    typedef tangent_intersection<T> tang_t;
    const point_t& p = static_cast<const point_t&>(d);
    const tang_t& ti = static_cast<const tang_t&>(d);
    out << '(' << p << ',' << ti << ')';

    return out;
}


// Usage:
//
// prior(end(datas))->z_ = tangent(prior(end(datas))),
// adjacent_difference(
//  make_reverse_iterator(end(datas)),
//  make_reverse_iterator(begin(datas)),
//  update_z<T>()
// );

template<typename T>
struct update_tangent_intersection{
    typedef point<T>            point_;
    typedef const point_&       first_argument_type;
    typedef const point_&       second_argument_type;
    typedef data<T>             result_type;

    result_type
    operator()(first_argument_type a,second_argument_type b)const{
        result_type result(a,tangent_intersection<T>(a,b));
        return result;
    }
};

struct pred_isinf{
    typedef bool result_type;
    // Used by find_if(begin(datas_),end(datas_),pred_isinf)
    // because had problem with bind etc. (reason: mutable data?)
    template<typename D>
    result_type operator()(D& d)const{
        return math::isinf(d.cum_sum_);
    }
};


}// ars
}// detail
}// statistics
}// boost

#endif
