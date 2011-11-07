///////////////////////////////////////////////////////////////////////////////
// vector_space::functional::l1_distance.hpp                         //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_VECTOR_SPACE_FUNCTIONAL_l1_DISTANCE_SQUARED_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_VECTOR_SPACE_FUNCTIONAL_l1_DISTANCE_SQUARED_HPP_ER_2009
#include <cmath>
#include <numeric>
#include <functional>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/function.hpp>
#include <boost/call_traits.hpp>
#include <boost/range.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/math/tools/precision.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace vector_space{

    // See l2_distance_squared
    template<typename R>
    class l1_distance{
        typedef typename is_reference<R>::type is_ref_;
    public:
        typedef typename remove_reference<R>::type const_range_type;
        typedef typename remove_const<const_range_type>::type range_type;
        typedef typename range_value<range_type>::type result_type;
                    
        // Constructor
        l1_distance(typename call_traits<R>::param_type x);
        l1_distance(const l1_distance& that);
        l1_distance& operator=(const l1_distance& that);                    

        // Call
        template<typename R1> 
        result_type operator()(const R1& y)const;
                        
        private:
        l1_distance();
        typename call_traits<R>::value_type x_;
    };
    
    template<typename R> 
    l1_distance<R> 
    make_l1_distance(const R& x){
        return l1_distance<R>(x);
    }

    // Definitions
    template<typename R>
    l1_distance<R>::l1_distance(
        typename call_traits<R>::param_type x
    ):x_(x){
        BOOST_MPL_ASSERT((mpl::not_<is_ref_>));
    }

    template<typename R>
    l1_distance<R>::l1_distance(
        const l1_distance& that
    ):x_(that.x_){}

    template<typename R>
    l1_distance<R>& 
    l1_distance<R>::operator=(const l1_distance& that){
        if(&that != this){
            BOOST_MPL_ASSERT((mpl::not_<is_ref_>));
            x_ = that.x_;
        }
        return *this;
    }
    
    template<typename R>
    template<typename R1> 
    typename l1_distance<R>::result_type 
    l1_distance<R>::operator()(const R1& y)const{
        BOOST_ASSERT( 
            size(x_) == size(y) 
        );
        
        typedef typename range_value<R1>::type y_;
        
        function<result_type(const result_type&)> fun = fabs;
        
        return std::inner_product(
            begin(y),
            end(y),
            begin(x_),
            static_cast<result_type>(0),
            std::plus<result_type>(),
            boost::lambda::bind(
                fun,
                (lambda::_1 - lambda::_2)
            )
        );
    };

}// vector_space
}// detail
}// statistics
}// boost

#endif