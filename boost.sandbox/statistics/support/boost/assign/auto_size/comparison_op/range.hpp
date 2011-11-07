//////////////////////////////////////////////////////////////////////////////
// assign::detail::comparison_op::range.hpp                                 //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_COMPARISON_OP_RANGE_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_COMPARISON_OP_RANGE_ER_2010_HPP
#include <boost/assign/auto_size/comparison_op/crtp.hpp>
#include <boost/assign/auto_size/comparison_op/completed_policy.hpp>

namespace boost{
namespace assign{
namespace detail{
namespace range_comparison_op{

    // Let G={ D : D derives from base_of<D>::type }  
    // Let L and R model Range. For either of (L xor R) in G or (L and R) in G, 
    // Expression      Returns
    //   l == r        ::boost::iterator_range_detail::equal( l, r )
    //   l < r         ::boost::iterator_range_detail::less_than( l, r )
    // The remaining operators, !=, >,<=,>= are defined using their 
    // relationships to the two fundamental ones.
    struct fundamental_policy{
    
        template<typename L,typename R>
        static bool equal(const L& l,const R& r){
            return ::boost::iterator_range_detail::equal( l, r );
        }
        template<typename L,typename R>
        static bool less(const L& l,const R& r){
            return ::boost::iterator_range_detail::less_than( l, r );
        }

    };

    template<typename D>
    struct base_of{
        typedef comparison_op::crtp<
            D,
            comparison_op::completed_policy<
                range_comparison_op::fundamental_policy
            >
        > type;
    };

}// range_comparison_op
}// detail
}// assign
}// boost

#endif

