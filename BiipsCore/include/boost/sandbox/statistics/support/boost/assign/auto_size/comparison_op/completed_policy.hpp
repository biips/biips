//////////////////////////////////////////////////////////////////////////////
// assign::detail::comparison_op::completed_policy.hpp                      //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_COMPARISON_OP_COMPLETED_POLICY_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_COMPARISON_OP_COMPLETED_POLICY_ER_2010_HPP

namespace boost{
namespace assign{
namespace detail{
namespace comparison_op{

    // If P is a 'fundamental policy' i.e. it defines
    //     P::equal(l,r)
    //     P::less(l,r)
    // then Q = completed_policy<P> defines, in addition to those above,
    //     Q::not_equal(l,r)
    //     Q::greater(l,r)
    //     Q::less_equal(l,r)
    //     Q::greater_equal(l,r)
    // using their logical relationships.
    template<typename P>
    struct completed_policy : P{
        template<typename L,typename R>
        static bool not_equal(const L& l,const R& r){
            return !(P::equal(l,r));
        }
        template<typename L,typename R>
        static bool less_equal(const L& l,const R& r){
            return (P::less(l,r)) || (P::equal(l,r));
        }

        template<typename L,typename R>
        static bool greater(const L& l,const R& r){
            return !(less_equal(l,r));
        }

        template<typename L,typename R>
        static bool greater_equal(const L& l,const R& r){
            return !(P::less(l,r));
        }

    };

}// comparison_op
}// detail
}// assign
}// boost

#endif
