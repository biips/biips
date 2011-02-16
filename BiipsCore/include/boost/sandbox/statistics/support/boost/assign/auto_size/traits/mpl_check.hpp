//////////////////////////////////////////////////////////////////////////////
// assign::detail::traits::conversion_mpl_check.hpp                         //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_TRAITS_CONVERSION_MPL_CHECK_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_TRAITS_CONVERSION_MPL_CHECK_ER_2010_HPP
#include <boost/mpl/assert.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/array.hpp>
#include <boost/type_traits.hpp>
#include <boost/assign/auto_size/chain/reference_traits.hpp>

namespace boost{
namespace assign{
namespace detail{

namespace conversion_mpl_check{

    template<typename T,bool is = true>
    struct assert_cond{
        BOOST_MPL_ASSERT((T));    
    };

    template<typename T>
    struct assert_cond<T,false>{
        BOOST_MPL_ASSERT_NOT((T));    
    };

    template<typename T,typename U,bool is = true>
    struct assert_eq  
        : assert_cond<boost::is_same<T,U>,is>{};

namespace apply_conversion{

    template<typename T>
    struct helper : reference_traits::convert_to<T> {};

    template<typename T,typename U,bool is = true>
    struct assert_eq 
        : conversion_mpl_check::assert_eq<typename helper<T>::type,U,is>
    {};

}

// e.g. 
// conversion_mpl_check::verify_wrapper<int,detail::assign_reference_copy>();

template<typename T,template<typename> class W> 
void verify_wrapper()
{  
        
    typedef T          v_;
    typedef const v_   cv_;
    typedef v_&        r_;
    typedef cv_&       cr_;

    typedef W<v_>      wv_;
    typedef const wv_  cwv_;
    typedef wv_&       rwv_;
    typedef cwv_&      crwv_;

    typedef W<cv_>     wcv_;
    typedef const wcv_ cwcv_;
    typedef wcv_&      rwcv_;
    typedef cwcv_&     crwcv_;
            
    apply_conversion::assert_eq< v_,    v_   >();
    apply_conversion::assert_eq< cv_,   cv_  >();
    apply_conversion::assert_eq< r_,    r_   >();
    apply_conversion::assert_eq< cr_,   cr_  >();

    apply_conversion::assert_eq< wv_,   r_   >();
    apply_conversion::assert_eq< wcv_,  cr_  >();

    apply_conversion::assert_eq< cwv_,  r_   >();
    apply_conversion::assert_eq< cwcv_, cr_  >();

    apply_conversion::assert_eq< rwv_,  r_   >();
    apply_conversion::assert_eq< rwcv_, cr_  >();

    apply_conversion::assert_eq< crwv_,  r_  >();
    apply_conversion::assert_eq< crwcv_, cr_ >();
  
}

}// conversion_mpl_check
}// detail
}// assign
}// boost

#endif
