//////////////////////////////////////////////////////////////////////////////
// assign::detail::range::chain_l.hpp                                       //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_RANGE_CHAIN_L_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_RANGE_CHAIN_L_ER_2010_HPP
#include <boost/mpl/identity.hpp>
#include <boost/mpl/void.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/assign/auto_size/range/hold_converted_range.hpp>
#include <boost/assign/auto_size/range/converter.hpp>
#include <boost/assign/auto_size/range/basic_chain.hpp>
#include <boost/assign/auto_size/range/chain_common.hpp>
#include <boost/assign/auto_size/range/chain_r.hpp>

namespace boost{
namespace assign{
namespace detail{
namespace chain_impl{

    // ---- template parameters ---- //

    // L : converted_range::list<>
    // E : basic_chain_impl::expr<> 
    // Rng1 : a range to chain to 
    // is_first : indicator
    // V : a value conversion parameter
    // R : a reference conversion parameter

    // ---- list ---- //

    template<typename L,typename Rng1,bool is_first,typename V,typename R>
    struct list_of_expr_l : boost::mpl::identity<
       converted_range::list<L,Rng1,is_first,V,R>
    >{};
 
    // ---- chainer ---- //
    template<typename L,typename E,typename Rng1,
        bool is_first,typename V,typename R>
    class expr_l : 
        list_of_expr_l<L,Rng1,is_first,V,R>::type,
        public impl_of_expr<E,Rng1,is_first,V,R,false>::type
    {
        typedef expr_l<L,E,Rng1,is_first,V,R> this_;

        public:
        
        // bases
        typedef typename list_of_expr_l<L,Rng1,is_first,V,R>::type list_;
        typedef typename impl_of_expr<E,Rng1,
           is_first,V,R,false>::type impl_;
                
        // types
        typedef V conversion_value;
        typedef R conversion_reference;

        explicit expr_l(Rng1& r1):list_(r1),impl_(this->converted_range){}
        explicit expr_l(L& l,E& e,Rng1& r1):list_(l,r1),
            impl_(e,this->converted_range){}

        // unary operators
        template<typename Rng2>
        struct result_impl 
            { typedef expr_l<list_,impl_,Rng2,false,V,R> type; };
  

        template<typename Rng2> // Warning : overrides base.
        typename result_impl<Rng2>::type
        operator()(Rng2& r2)
        {
            typedef typename result_impl<Rng2>::type res_;
            return res_(
                static_cast<list_&>(*this),
                static_cast<impl_&>(*this),
                r2
            );            
        }

        template<typename Rng2> 
        typename result_impl<Rng2>::type
        operator&&(Rng2& r2){
            return (*this)(r2);
        }

//        template<typename Rng2>
//        typename assign::result_of::chain_operator_r<this_,Rng2>::type 
//        operator&&(const Rng2& r2){
//            return chain_r(*this)(r2);
//        }


        BOOST_ASSIGN_AS_CONVERTER(this_)
        
    };
 
    template<typename T>
    struct is_expr_l : boost::mpl::bool_<false>{};

    template<typename L,typename E,typename Rng1,
        bool is_first,typename V,typename R>
    struct is_expr_l<expr_l<L,E,Rng1,is_first,V,R> >:boost::mpl::bool_<true>{};

}// chain_impl
}// detail

namespace result_of{

    template<typename Rng1,typename V,typename R 
        = typename detail::convert_range_reference<Rng1,V>::type>
    struct chain_l{ 
        typedef boost::mpl::void_ top_;
        typedef detail::chain_impl::expr_l<top_,top_,Rng1,1,V,R> type; 
        typedef typename detail::basic_chain_impl::sel_const<
            Rng1,false>::type cr1_;
        static type call(cr1_& r1){ return type(r1); }
    };

    template<typename Rng1>
    struct deduced_chain_l : result_of::chain_l<
        Rng1, typename detail::chain_impl::deduce_value<Rng1,false>::type
    >{};

    template<typename R1,typename R2>
    struct chain_operator_l{
        typedef typename 
            result_of::deduced_chain_l<R1>::type first_; 
        typedef typename first_::template result_impl<R2>::type type;

    };

}// result_of

    template<typename V,typename R,typename Rng1>
    typename result_of::chain_l<Rng1,V,R>::type    
    chain_l(Rng1& r1,
        typename boost::disable_if<boost::is_same<R,use_default> >::type* = 0
    ){
        typedef result_of::chain_l<Rng1,V,R> caller_;
        return caller_::call(r1);
    }

    template<typename V,typename R,typename Rng1>
    typename result_of::chain_l<Rng1,V>::type    
    chain_l(Rng1& r1,
        typename boost::enable_if<boost::is_same<R,use_default> >::type* = 0
    ){
        typedef result_of::chain_l<Rng1,V> caller_;
        return caller_::call(r1);
    }

    template<typename Rng1>
    typename result_of::deduced_chain_l<Rng1>::type
    chain_l(Rng1& r1)
    {
        typedef result_of::deduced_chain_l<Rng1> caller_;
        return caller_::call(r1);
    }

    // Operator &&

    template<typename R1,typename R2>
    typename boost::lazy_disable_if<
         detail::chain_impl::is_expr_l<R1>,
         assign::result_of::chain_operator_l<R1,R2>
    >::type
    operator &&(R1& r1,R2& r2){
        return boost::assign::chain_l(r1)(r2);
    }

}// assign
}// boost

#endif