//////////////////////////////////////////////////////////////////////////////
// assign::detail::range::chain_r.hpp                                       //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_RANGE_CHAIN_R_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_RANGE_CHAIN_R_ER_2010_HPP
#include <boost/mpl/identity.hpp>
#include <boost/mpl/void.hpp>
#include <boost/assign/auto_size/range/converter.hpp>
#include <boost/assign/auto_size/range/basic_chain.hpp>
#include <boost/assign/auto_size/range/chain_common.hpp>

namespace boost{
namespace assign{
namespace detail{
namespace chain_impl{

    // ---- template parameters ---- //

    // E : basic_chain_impl::expr<> 
    // Rng1 : a range to chain to 
    // is_first : indicator
    // V : a value conversion parameter
    // R : a reference conversion parameter

    // ---- chainer ---- //
    template<typename E,typename Rng1,
        bool is_first,typename V,typename R>
    class expr_r : 
        public impl_of_expr<E,Rng1,is_first,V,R,true>::type
    {
        typedef expr_r<E,Rng1,is_first,V,R> this_;

        public:
        
        // bases
        typedef typename impl_of_expr<E,Rng1,is_first,V,R,true>::type impl_;
                
        // types
        typedef V conversion_value;
        typedef R conversion_reference;

        explicit expr_r(const Rng1& r1)
            :impl_(convert_range<V,R,Rng1>(r1))
        {}
        explicit expr_r(const E& e,const Rng1& r1)
            :impl_(e,convert_range<V,R,Rng1>(r1))
        {}
                
        // unary operators
        template<typename Rng2>
        struct result_impl 
            { typedef expr_r<impl_,Rng2,false,V,R> type; };
  
        template<typename Rng2> // Warning : overrides base.
        typename result_impl<Rng2>::type
        operator()(const Rng2& r2)const
        {
            typedef typename result_impl<Rng2>::type res_;
            return res_(
                static_cast<const impl_&>(*this),
                r2
            );            
        }

        BOOST_ASSIGN_AS_CONVERTER(this_)
        
    };

}// chain_impl
}// detail
namespace result_of{

    template<typename Rng1,typename V,typename R = typename 
        detail::convert_range_reference<
            const Rng1,typename boost::add_const<V>::type>::type>
    struct chain_r{ 
        typedef boost::mpl::void_ top_;
        typedef detail::chain_impl::expr_r<top_,Rng1,1,V,R> type; 
        typedef typename detail::basic_chain_impl::sel_const<
            Rng1,true>::type cr1_;
        static type call(cr1_& r1){ return type(r1); }
    };

    template<typename Rng1>
    struct deduced_chain_r : result_of::chain_r<
        Rng1, 
        typename detail::chain_impl::deduce_value<const Rng1,true>::type
    >{};


    template<typename R1,typename R2>
    struct chain_operator_r{
        typedef typename 
            result_of::deduced_chain_r<R1>::type first_; 
        typedef typename first_::template result_impl<R2>::type type;
    };

}

    template<typename V,typename R,typename Rng1>
    typename result_of::chain_r<Rng1,V,R>::type    
    chain_r(const Rng1& r1,
        typename boost::disable_if<boost::is_same<R,use_default> >::type* = 0
    ){
        typedef result_of::chain_r<Rng1,V,R> caller_;
        return caller_::call(r1);
    }

    template<typename V,typename R,typename Rng1>
    typename result_of::chain_r<Rng1,V>::type    
    chain_r(const Rng1& r1,
        typename boost::enable_if<boost::is_same<R,use_default> >::type* = 0
    ){
        typedef result_of::chain_r<Rng1,V> caller_;
        return caller_::call(r1);
    }

    template<typename Rng1>
    typename result_of::deduced_chain_r<Rng1>::type    
    chain_r(const Rng1& r1)
    {
        typedef result_of::deduced_chain_r<Rng1> caller_;
        return caller_::call(r1);
    }

    // Operator &&

    template<typename R1,typename R2>
    typename assign::result_of::chain_operator_r<R1,R2>::type    
    operator &&(const R1& r1,const R2& r2){
        return boost::assign::chain_r(r1)(r2);
    }

    template<typename E,typename Rng1,bool is_first,
        typename V,typename R,typename Rng2>
    typename detail::chain_impl::expr_r<E,Rng1,is_first,V,R>
        ::template result_impl<Rng2>::type    
    operator &&(
        const detail::chain_impl::expr_r<E,Rng1,is_first,V,R>& r1,
        const Rng2& r2
    ){
        return r1(r2);
    }

}// assign
}// boost

#endif