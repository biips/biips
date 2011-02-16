///////////////////////////////////////////////////////////////////////////////
// ars::search_reflection.hpp                                                //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_SEARCH_REFLECTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_SEARCH_REFLECTION_HPP_ER_2009
#include <string>
#include <boost/format.hpp>
#include <boost/function.hpp>
#include <boost/ars/constant.hpp>
#include <boost/ars/point.hpp>
#include <boost/ars/error.hpp>
#include <boost/ars/function/signature.hpp>
#include <boost/ars/function/adaptor.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{

// This function searches by reflection for initial starting points 
// (x_0,x_1) such that if x_min = -inf, dy_0>0 and if x_min = inf, dy_0<0

//TODO even if x_min or x_max finite, |dy|>eps might be desirable
template<typename T>
unsigned
search_reflection(
    const T& x_min,
    const T& x_max,
    boost::function<typename ars::function::signature<T>::type> delegate,
    point<T>& p_0,
    point<T>& p_1,
    unsigned n_max
){
    static const char* function
    = "search_reflection(%1%, %2%, ...)";

    typedef point<T> point_t;
    typedef constant<T> const_;

    struct local{
        static bool ok_0(const point_t& p){ 
            return ( p.dy() >=  const_::eps_ ); }
        static bool ok_1(const point_t& p){ 
            return ( p.dy() <= (-const_::eps_) ); }
    };

    unsigned n = 0;
    T new_x_0 = p_0.x();
    T new_x_1 = p_1.x();
    bool ok_0, ok_1 = true;
    if(math::isinf(x_min)){ ok_0 = local::ok_0(p_0); }
    if(math::isinf(x_max)){ ok_1 = local::ok_1(p_1); }

    while(
        (!(ok_0 && ok_1))
    ){
        if(n>n_max){
            boost::format f(function);
            f % x_min % x_max;
            throw ars::exception(f.str(),"n>n_max",p_0,p_1);
        }

        if(!ok_0){
            T delta = (new_x_1-new_x_0);
            if(delta < const_::eps_ ){
                boost::format f(function);
                f % x_min % x_max;
                throw ars::exception(
                    f.str(),
                    "new_x_0-new_x_1< (- const_::eps_)",
                    p_0,p_1
                );
            }
            //TODO max(new_x_0,-highest) ?
            new_x_0 -=  delta;
            p_0 = create_point<T>(new_x_0,delegate);
            ok_0 = local::ok_0(p_0);
        }
        if(!ok_1){
            T delta = (new_x_1-new_x_0);
            if( delta < const_::eps_){
                boost::format f(function);
                f % x_min % x_max;
                throw ars::exception(
                    f.str(),
                    "new_x_1-new_x_0 > const_::eps_",
                    p_0,p_1
                );
            }
            new_x_1 += delta;
            p_1 = create_point<T>(new_x_1,delegate);
            ok_1 = local::ok_1(p_1);
        }
        ++n;
    }
    return n;
}

template<typename T>
unsigned search_reflection(
    const T& x_min,
    const T& x_max,
    boost::function<typename ars::function::signature<T>::type> delegate,
    const T& x_0,
    const T& x_1,
    point<T>& p_0,
    point<T>& p_1,
    unsigned n_max
){
    {
        p_0 = create_point<T>(x_0,delegate);
        p_1 = create_point<T>(x_1,delegate);
    }
    return search_reflection(
        x_min,
        x_max,
        delegate,
        p_0,
        p_1,
        n_max
    );
}

// TODO theck that
// T = remove_const< remove_reference< D> ::type >::type ::value_type
template<typename D,typename T> // D = const E& or E
unsigned search_reflection_dist(
    const T& x_min,
    const T& x_max,
    const D& dist,
    const T& x_0,
    const T& x_1,
    ars::point<T>& p_0,
    ars::point<T>& p_1,
    unsigned n_max
){
    typedef ars::function::adaptor<const D&> fnal_t;
    typedef typename ars::function::signature<T>::type   signature;
    typedef boost::function<signature>                  delegate_t;
    fnal_t fnal(dist);

    return search_reflection<T>(
        x_min,
        x_max,
        fnal, //automatic conversion
        x_0,
        x_1,
        p_0,
        p_1,
        n_max
    );
}


}// ars
}// detail
}// statistics
}// boost

#endif
