///////////////////////////////////////////////////////////////////////////////
// math::shortcut::shortcut.hpp                                              //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_MATH_CONSTANT_SHORTCUT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_MATH_CONSTANT_SHORTCUT_HPP_ER_2009
#include <limits>
#include <iostream>
#include <boost/mpl/bool.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/math/tools/precision.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/type_traits.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace math{
namespace constant{
        
        template<typename T>
        class shortcut{
         public:
            typedef boost::numeric::converter<T,unsigned> conv_t;
            typedef boost::numeric::bounds<T> bounds_;
            
            static const T zero;
            static const T eps;
            static const T one;
            static const T two;
            static const T lmin; //exp(lmin_)>eps
            static const T lmax; //exp(lmax_)<inf
            static const T inf;
            static const T highest;
            static const T low;
            static const T has_inf;
            static const T inf_else_highest;
            static const T quiet_nan;
            static const T sig_nan;
            
         private:
            static T inf_else_highest_impl(boost::mpl::bool_<true>)
            {
                return inf;
            }
            static T inf_else_highest_impl(boost::mpl::bool_<false>)
            {
                return highest;
            }
            
            
        };
        
        template<typename T>
        const T shortcut<T>::has_inf = std::numeric_limits<T>::has_infinity;

        template<typename T>
        const T shortcut<T>::zero = conv_t::convert(0);

        template<typename T>
        const T shortcut<T>::eps = boost::math::tools::epsilon<T>();

        template<typename T>
        const T shortcut<T>::one = conv_t::convert(1);

        template<typename T>
        const T shortcut<T>::two = conv_t::convert(2);

        template<typename T>
        const T shortcut<T>::highest = shortcut<T>::bounds_::highest();

        template<typename T>
        const T shortcut<T>::low = shortcut<T>::bounds_::lowest();

        template<typename T>
        const T shortcut<T>::lmin = boost::math::tools::log_min_value<T>();

        template<typename T>
        const T shortcut<T>::lmax = boost::math::tools::log_max_value<T>();

        template<typename T>
        const T shortcut<T>::inf = std::numeric_limits<T>::infinity();

        template<typename T>
        const T shortcut<T>::quiet_nan = std::numeric_limits<T>::quiet_NaN();

        template<typename T>
        const T shortcut<T>::sig_nan = std::numeric_limits<T>::signaling_NaN();
        
        template<typename T>
        const T shortcut<T>::inf_else_highest 
            = (shortcut<T>::has_inf) ? shortcut<T>::inf : shortcut<T>::highest;

}// constant
}// math
}// detail
}// statistics
}// boost

#endif
