//////////////////////////////////////////////////////////////////////////////
// assign::detail::comparison_op::crtp.hpp                                  //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_COMPARISON_OP_CRTP_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_COMPARISON_OP_CRTP_ER_2010_HPP
#include <boost/mpl/and.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/type_traits/is_base_of.hpp> 
#include <boost/type_traits/detail/yes_no_type.hpp>

namespace boost{
namespace assign{
namespace detail{
namespace comparison_op{

// Source:
// http://groups.google.com/group/comp.lang.c++.moderated/browse_thread/thread/389d8fe278ef0b13#

    // Let G(P)={ D : D derives from crtp<D,P> }  
    // For either of (L xor R) in G(P) or (L and R) in G(P), 
    // Expression                         Returns
    //   operator == (const L&,const R&)  P::equal(l,r)
    //   operator != (const L&,const R&)  P::not_equal(l,r)
    //   operator < (const L&,const R&)   P::less(l,r)
    //   operator > (const L&,const R&)   P::greater(l,r)
    //   operator <= (const L&,const R&)  P::less_equal(l,r)
    //   operator >= (const L&,const R&)  P::greater_equal(l,r)
    template<typename D,typename P> 
    struct crtp{
        struct comparison_op_traits{
            typedef P policy_;
            typedef D derived_;
        };
    };

namespace traits{

    template<typename T>
    struct policy_of{ 
        typedef typename T::comparison_op_traits::policy_ type; 
    };

    template<typename T>
    struct derived_of{ 
        typedef typename T::comparison_op_traits::derived_ type; 
    };

    template<typename T>
    struct has_traits{

        template<typename U> 
        static type_traits::yes_type test(typename U::comparison_op_traits*);
        template<typename U> 
        static type_traits::no_type test(...);
         
        BOOST_STATIC_CONSTANT( bool, 
            value = sizeof( test<T>(0) ) == sizeof(type_traits::yes_type) );
        typedef boost::mpl::bool_<value> type;
    };

    template<typename T> 
    struct is_base_of_impl : boost::is_base_of<
        crtp<
            typename derived_of<T>::type,
            typename policy_of<T>::type
        >,
        T
    >{};

    template<typename T>
    struct is_base_of : boost::mpl::eval_if< 
        traits::has_traits<T>,
        traits::is_base_of_impl<T>, // safety check
        boost::mpl::identity< boost::mpl::bool_<false> >
    >{};

    template<typename T,bool value>
    struct is_base_of_equal_to : boost::mpl::equal_to<
        typename traits::is_base_of<T>::type,
        boost::mpl::bool_<value>
    >{};

    template<typename D1,bool value1,typename D2,bool value2>
    struct are_bases_of_equal_to : boost::mpl::and_<
        typename traits::is_base_of_equal_to<D1,value1>::type,
        typename traits::is_base_of_equal_to<D2,value2>::type
    >{};
    
}//traits
namespace forward{

    struct equal{
        template<typename P,typename L,typename R>
        static bool call(const L& l,const R& r){
            return (P::equal(l,r));
        }
    };

    struct not_equal{
        template<typename P,typename L,typename R>
        static bool call(const L& l,const R& r){
            return (P::not_equal(l,r));
        }
    };

    struct less{
        template<typename P,typename L,typename R>
        static bool call(const L& l,const R& r){
            return (P::less(l,r));
        }
    };

    struct greater{
        template<typename P,typename L,typename R>
        static bool call(const L& l,const R& r){
            return (P::greater(l,r));
        }
    };
    
    struct less_equal{
        template<typename P,typename L,typename R>
        static bool call(const L& l,const R& r){
            return (P::less_equal(l,r));
        }
    };

    struct greater_equal{
        template<typename P,typename L,typename R>
        static bool call(const L& l,const R& r){
            return (P::greater_equal(l,r));
        }
    };

}//forward

#define BOOST_ASSIGN_DETAIL_COMPARISON_OP(SYMB,OP)                            \
    template<typename L,typename R>                                           \
    typename boost::enable_if_c<                                              \
        traits::are_bases_of_equal_to<L,true,R,false>::value,                 \
        bool                                                                  \
    >::type                                                                   \
    operator SYMB (const L& l,const R& r){                                    \
        typedef typename traits::policy_of<L>::type policy_;                  \
        typedef typename traits::derived_of<L>::type derived_;                \
        return OP::call<policy_>(static_cast<const derived_&> (l),r);         \
    }                                                                         \
                                                                              \
    template<typename L,typename R>                                           \
    typename boost::enable_if_c<                                              \
        traits::are_bases_of_equal_to<L,false,R,true>::value,                 \
        bool                                                                  \
    >::type                                                                   \
    operator SYMB(const L& l,const R& r){                                     \
        typedef typename traits::policy_of<R>::type policy_;                  \
        typedef typename traits::derived_of<R>::type derived_;                \
        return OP::call<policy_>(l,static_cast<const derived_&> (r));         \
    }                                                                         \
                                                                              \
    template<typename L,typename R>                                           \
    typename boost::enable_if_c<                                              \
        traits::are_bases_of_equal_to<L,true,R,true>::value,                  \
        bool                                                                  \
    >::type                                                                   \
    operator SYMB(const L& l,const R& r){                                     \
        typedef typename traits::policy_of<L>::type policy1_;                 \
        typedef typename traits::policy_of<R>::type policy2_;                 \
        BOOST_MPL_ASSERT((boost::is_same<policy1_,policy2_>));                \
        typedef typename traits::derived_of<L>::type derived1_;               \
        typedef typename traits::derived_of<R>::type derived2_;               \
        return OP::call<policy1_>(                                            \
            static_cast<const derived1_&> (l),                                \
            static_cast<const derived2_&> (r)                                 \
        );                                                                    \
    }                                                                         \
/**/    

BOOST_ASSIGN_DETAIL_COMPARISON_OP( == , comparison_op::forward::equal )
BOOST_ASSIGN_DETAIL_COMPARISON_OP( != , comparison_op::forward::not_equal )
BOOST_ASSIGN_DETAIL_COMPARISON_OP( <  , comparison_op::forward::less )
BOOST_ASSIGN_DETAIL_COMPARISON_OP( >  , comparison_op::forward::greater )
BOOST_ASSIGN_DETAIL_COMPARISON_OP( <= , comparison_op::forward::less_equal )
BOOST_ASSIGN_DETAIL_COMPARISON_OP( >= , comparison_op::forward::greater_equal )

}// comparison_op
}// detail
}// assign
}// boost

#endif
