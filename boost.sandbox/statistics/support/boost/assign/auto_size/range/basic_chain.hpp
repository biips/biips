//////////////////////////////////////////////////////////////////////////////
// assign::detail::basic_chain.hpp                                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard - Manuel Peinado Gallego               //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_RANGE_BASIC_CHAIN_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_RANGE_BASIC_CHAIN_ER_2010_HPP
#include <boost/mpl/if.hpp>
#include <boost/mpl/void.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/range/chain.hpp>
#include <boost/assign/auto_size/range/result_of_chain.hpp>
#include <boost/assign/auto_size/range/hold_previous.hpp>

// This is based on
// http://gist.github.com/287791
// developed by MPG, but allows lvalues.

// Usage : Let r1, r2 and r3 denote lvalues of ranges,
//     boost::copy( input, boost::begin( basic_chain_l(r1)(r2)(r3) ) );
// Note: r1, r2, r3 cannot be a mixture of ranges of values and ranges of 
// reference wrappers, hence the prefix 'basic'. 

// TODO operator| as in RangeEx

namespace boost{
namespace assign{
namespace detail{
namespace basic_chain_impl{

    template<typename E,typename R1,bool is_first,bool add_const> class expr;

    template<typename E,typename R2,bool add_const>
    struct next_expr{
        typedef expr<E,R2,false,add_const> type;
    };

    template<typename T,bool b>
    struct sel_const 
       : boost::mpl::if_c<b,typename boost::add_const<T>::type,T>{};

    template<typename E,typename R1,bool is_first,bool add_const>
    struct facade_of_expr{
        typedef typename E::facade_ facade_;
        typedef typename sel_const<facade_,add_const>::type sel_facade_;
        typedef typename sel_const<R1,add_const>::type sel_r1_;
        typedef typename result_of::chain<sel_facade_,sel_r1_>::type type;
    };

    template<typename E,typename R1,bool add_const>
    struct facade_of_expr<E,R1,true,add_const>{
        typedef typename sel_const<R1,add_const>::type sel_r1_;
        typedef boost::sub_range<sel_r1_> type;
    };
                      
    template<typename E,bool is_first,bool add_const>
    struct sel_hold_previous : boost::mpl::if_c<
        add_const,
        boost::mpl::empty_base,
        assign::detail::hold_previous<E,is_first>
    >{};

                      
    template<typename E,typename R1,bool is_first,bool add_const>
    class expr : 
        sel_hold_previous<E,is_first,add_const>::type, 
        public basic_chain_impl::facade_of_expr<E,R1,is_first,add_const>::type
    {
        protected:
        typedef expr<E,R1,is_first,add_const> this_;

        typedef typename sel_hold_previous<E,is_first,add_const>::type 
            hold_previous_;         

        typedef typename sel_const<E,add_const>::type sel_e_;
        typedef typename sel_const<R1,add_const>::type sel_r1_;
      
        template<typename T,bool b> //a dependence on T is needed
        struct enable : boost::enable_if_c<b>{};

        public:

        typedef typename 
            basic_chain_impl::facade_of_expr<E,R1,is_first,add_const>::type facade_;
                    
        facade_& facade(){ return (*this); }
        const facade_& facade()const{ return (*this); }
    
        // lvalue constructors
        explicit expr(R1& r1):facade_(r1){}
        explicit expr(E& p, R1& r1)
            :hold_previous_(p),
            facade_(boost::chain(this->previous.facade(),r1))
            { } 

        // rvalue constructors
        explicit expr(const R1& r1):facade_(r1){}
        explicit expr(const E& p, const R1& r1)
            :facade_(boost::chain(p.facade(),r1))
            { } 

        // unary operators
        template<typename R2>
        struct result_impl : basic_chain_impl::next_expr<this_,R2,add_const>{};

        template<typename R2>
        typename result_impl<R2>::type 
        operator()(R2& r2,typename enable<R2,!add_const>::type* = 0)
        {
            typedef typename result_impl<R2>::type res_;
            return res_(*this,r2);
        }

        template<typename R2>
        typename result_impl<R2>::type 
        operator()(const R2& r2,typename enable<R2,add_const>::type* = 0)const
        {
            typedef typename result_impl<R2>::type res_;
            return res_(*this,r2);
        }
        
    };

    template<typename R1,bool add_const>
    struct first_expr{ 
        typedef boost::mpl::void_ top_;
        typedef basic_chain_impl::expr<top_,R1,true,add_const> type; 
        typedef typename sel_const<R1,add_const>::type sel_r1_;
        static type call(sel_r1_& r1){ return type(r1); }
    };

    
}// basic_chain_impl
}// detail

    template<typename R1>
    typename detail::basic_chain_impl::first_expr<R1,false>::type    
    basic_chain_l(R1& r1){
        typedef detail::basic_chain_impl::first_expr<R1,false> caller_;
        return caller_::call(r1);
    }

    template<typename R1>
    typename detail::basic_chain_impl::first_expr<R1,true>::type    
    basic_chain_r(const R1& r1){
        typedef detail::basic_chain_impl::first_expr<R1,true> caller_;
        return caller_::call(r1);
    }

}// range
}// boost

#endif