//////////////////////////////////////////////////////////////////////////////
// assign::detail::range::chain_common.hpp                                  //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_RANGE_CHAIN_COMMON_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_RANGE_CHAIN_COMMON_ER_2010_HPP
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/inherit.hpp>
#include <boost/mpl/void.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/range/chain.hpp>
#include <boost/assign/auto_size/range/convert_range.hpp>
#include <boost/assign/auto_size/traits/reference_traits.hpp>
#include <boost/assign/auto_size/reference_wrapper/conversion_traits.hpp>
#include <boost/assign/auto_size/range/result_of_chain.hpp>
#include <boost/assign/auto_size/range/hold_previous.hpp>
#include <boost/assign/auto_size/range/hold_converted_range.hpp>
#include <boost/assign/auto_size/range/basic_chain.hpp>
#include <boost/assign/auto_size/range/converter.hpp>

#include <boost/mpl/assert.hpp> // tmp
#include <boost/type_traits/is_reference.hpp> // tmp

// Usage : Let r1, r2 and r3 denote lvalue-ranges.
//     boost::copy( input, boost::begin( chain_l(r1)(r2)(r3) ) );
// This function does not have the restriction of basic_chain_l that none or all
// of r1, r2, r3 must be reference wrappers.

// TODO operator| as in RangeEx

namespace boost{
namespace assign{
namespace detail{
namespace chain_impl{

    // ---- template parameters ---- //

    // L : converted_range::list<> or empty base if rvalue 
    // E : basic_chain_impl::expr<> 
    // Rng1 : a range to chain to 
    // is_first : indicator
    // V : a value conversion parameter
    // R : a reference conversion parameter

    // ---- fwd decl ---- //
    template<typename L,typename E,typename Rng1,
        bool is_first,typename V,typename R,bool add_const> class expr;


    // ---- traits ---- //

    // Returns the implementation of expr<L,E,Rng1,is_first,V,R> for any L
    template<typename E,typename Rng1,
        bool is_first,typename V,typename R,bool add_const>
    struct impl_of_expr
    {
        typedef typename basic_chain_impl::sel_const<Rng1,add_const>::type cr1_;
        typedef typename 
            detail::result_of::convert_range<cr1_,V,R>::type conv_r1_;
        typedef typename E::template result_impl<conv_r1_>::type type;
    }; 

    template<typename E,typename Rng1,typename V,typename R,bool add_const>
    struct impl_of_expr<E,Rng1,true,V,R,add_const>
    {
        typedef typename basic_chain_impl::sel_const<Rng1,add_const>::type cr1_;
        typedef typename result_of::convert_range<cr1_,V,R>::type conv_r1_;
        typedef typename basic_chain_impl::first_expr<conv_r1_,add_const>::type type;
    };

    template<typename Rng1,bool add_const>
    struct deduce_value{
        typedef typename boost::range_reference<Rng1>::type from_;
        typedef typename reference_traits::convert_to<from_>::type ref_to_;
        typedef typename boost::remove_reference<ref_to_>::type to_;
        typedef typename basic_chain_impl::sel_const<to_,add_const>::type type;
    };

}// chain_impl
}// detail
}// assign
}// boost

#endif