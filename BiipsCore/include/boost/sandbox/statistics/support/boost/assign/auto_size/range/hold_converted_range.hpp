//////////////////////////////////////////////////////////////////////////////
// assign::detail::range::chain_l.hpp                                       //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_RANGE_HOLD_CONVERTED_RANGE_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_RANGE_HOLD_CONVERTED_RANGE_ER_2010_HPP
#include <boost/mpl/if.hpp>
#include <boost/mpl/void.hpp>
#include <boost/assign/auto_size/range/convert_range.hpp>
#include <boost/assign/auto_size/range/hold_previous.hpp>

namespace boost{
namespace assign{
namespace detail{

namespace converted_range{

    template<typename Rng,typename V,typename R>
    class holder{
        typedef typename 
            detail::result_of::convert_range<Rng,V,R>::type storage_;

        public:

        holder(){} 
        holder(Rng& r):converted_range(convert_range<V,R>(r)){} 
        
        mutable storage_ converted_range;
    };

    template<typename E,typename Rng1,bool is_first,typename V,typename R>
    class list 
        : public hold_previous<E,is_first>, 
        public holder<Rng1,V,R>
    {
        typedef list<E,Rng1,is_first,V,R> this_;
        typedef hold_previous<E,is_first> previous_;
        typedef converted_range::holder<Rng1,V,R> holder_;

        public:

        typedef V conversion_value;
        typedef R conversion_reference;
    
        list(Rng1& r1):holder_(r1){}
        list(E& e,Rng1& r1)
            :previous_(e),holder_(r1){}

        template<typename Rng2>
        struct result_of_next{ typedef list<this_,Rng2,is_first,V,R> type; };
    
        template<typename Rng2>
        typename result_of_next<Rng2>::type
        next(Rng2& r2){
            typedef typename result_of_next<Rng2>::type res_;
            return res_(*this,r2);
        }
    
    };


}// converted_range
}// detail
}// assign
}// boost

#endif
