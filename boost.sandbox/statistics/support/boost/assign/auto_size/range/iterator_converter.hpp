//////////////////////////////////////////////////////////////////////////////
// assign::detail::converted_iterator.hpp                                   //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_ITERATOR_CONVERTER_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_ITERATOR_CONVERTER_ER_2010_HPP
#include <boost/mpl/assert.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_traits.hpp>

// Usage:
//     convert_iterator<V,R>(i)
//     convert_iterator<V,use_default>(i)

namespace boost{
namespace assign{
namespace detail{

// Warning :
// If range_value<I>::type is const but V isn't, the convertible type is
// a value, not reference. If a reference is desired, V must also be const.
template<typename I,typename V>
struct converted_iterator_reference{

    typedef typename boost::iterator_reference<I>::type f_ref_;
    typedef typename boost::remove_reference<f_ref_>::type f_val_;
    typedef V t_val_;
    typedef typename boost::add_reference<V>::type t_ref_;
    typedef typename boost::is_convertible<f_ref_,t_ref_>::type use_ref_;
    typedef typename boost::mpl::if_<use_ref_,t_ref_,t_val_>::type type;
       
};

template<typename I, typename V,
    typename R = typename converted_iterator_reference<I,V>::type>
struct converted_iterator : boost::iterator_adaptor<
    detail::converted_iterator<I,V,R>           // Derived
    ,I                                          // Base
    ,V                                          // Value
    ,use_default                                // CategoryOrTraversal
    ,R                                          // Reference
    ,use_default                                // Difference
>{
    typedef boost::iterator_adaptor<
        detail::converted_iterator<I,V,R> 				
        ,I                         						
        ,V 		
        ,use_default 
        ,R 												
        ,use_default 
    > super_;
    
    converted_iterator(){}
    converted_iterator(const I& base):super_(base)
    {
        typedef typename boost::iterator_reference<I>::type i_ref_;
        typedef typename boost::is_convertible<i_ref_,R>::type necessary_;
        BOOST_MPL_ASSERT((necessary_));
    }
}; 
} //detail

template<typename V,typename R,typename I>
detail::converted_iterator<I,V,R>
convert_iterator(
    const I& i,
    typename boost::disable_if<boost::is_same<R,use_default> >::type* = 0
)
{
    typedef detail::converted_iterator<I,V,R> result_;
    return result_(i);
}

template<typename V,typename R,typename I>
detail::converted_iterator<I,V>
convert_iterator(
    const I& i,
    typename boost::enable_if<boost::is_same<R,use_default> >::type* = 0
)
{
    typedef detail::converted_iterator<I,V> result_;
    return result_(i);
}

}// assign
}// boost

#endif
