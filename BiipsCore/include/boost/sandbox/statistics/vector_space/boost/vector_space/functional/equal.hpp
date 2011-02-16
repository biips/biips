///////////////////////////////////////////////////////////////////////////////
// vector_space::functional::equal.hpp                                       //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_VECTOR_SPACE_FUNCTIONAL_EQUAL_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_VECTOR_SPACE_FUNCTIONAL_EQUAL_HPP_ER_2009
#include <boost/call_traits.hpp>
#include <ostream>
#include <boost/range.hpp>
#include <boost/mpl/nested_type.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/math/tools/precision.hpp>
#include <boost/vector_space/functional/l2_distance_squared.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace vector_space{

// This is a predicate for (x == y) iff (|| x - y || == 0).
//
// Requirements:
// F<R> distance(x);        Construction
// distance(y)              Returns an object of type range_value<R>::type
//
// Usage:
// functor<R,F> fun(x,eps);
// fun(y) returns true if distance(y) < eps
template<typename R, template<typename> class F = l2_distance_squared> 
class equal{
    typedef F<R> arg_;
    typedef typename mpl::nested_type<arg_>::type distance_type;    
    typedef typename remove_reference<R>::type const_range_type;
    typedef typename remove_const<const_range_type>::type range_type;
    typedef typename range_value<range_type>::type value_type;
public:
            
    //Construction
    equal(typename call_traits<R>::param_type x,value_type eps);
    equal(typename call_traits<R>::param_type x);
    equal(const equal& that);
    equal& operator=(const equal& that);
            
    //Evaluate
    typedef bool result_type;

    template<typename R1> result_type operator()(const R1& y)const;
    result_type epsilon()const;
                                               
//private:
    equal();
    distance_type distance_;
    value_type eps_;
    static value_type default_eps_;
};

// 
template<typename R, template<typename> class F>
std::ostream& operator<<(std::ostream& out,const equal<R,F>& e){
    out << '(' <<  e.epsilon() << ')';
    return out;
}

// Static members
template<typename R, template<typename> class F>
typename equal<R,F>::value_type
equal<R,F>::default_eps_ 
    = math::tools::epsilon<typename equal<R,F>::value_type>();

template<typename R, template<typename> class F>
typename equal<R,F>::result_type 
equal<R,F>::epsilon()const{
    return eps_;
}

//Construction

template<typename R, template<typename> class F>
equal<R,F>::equal(typename call_traits<R>::param_type x,value_type eps)
:distance_(x),eps_(eps){}

template<typename R, template<typename> class F>
equal<R,F>::equal(typename call_traits<R>::param_type x)
:distance_(x),eps_(default_eps_){}

template<typename R, template<typename> class F>
equal<R,F>::equal(const equal& that)
:distance_(that.distance_),eps_(that.eps_){}

template<typename R, template<typename> class F>
equal<R,F>& 
equal<R,F>::operator=(const equal& that){
    if(&that!=this){
        distance_ = that.distance_;
        eps_ = that.eps_;
    }
    return *this;
}
    
// Evaluate
template<typename R, template<typename> class F>
template<typename R1> 
typename equal<R,F>::result_type 
equal<R,F>::operator()(const R1& y)const{
    return (distance_(y)<eps_);
}

}// vector_space
}// detail
}// statistics
}// boost

#endif