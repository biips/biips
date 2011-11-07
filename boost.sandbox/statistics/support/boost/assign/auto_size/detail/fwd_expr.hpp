//////////////////////////////////////////////////////////////////////////////
// assign::detail::fwd_expr.hpp                                             //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_AUTO_SIZE_DETAIL_FWD_EXPR_ER_2010_HPP
#define BOOST_ASSIGN_AUTO_SIZE_DETAIL_FWD_EXPR_ER_2010_HPP
#include <boost/assign/auto_size/detail/types.hpp>

namespace boost{
namespace assign{
namespace detail{
namespace auto_size{
                        
    // ---- fwd declare ---- //

    template<typename E,typename T,int N,
        template<typename> class R,typename P>
    class expr;

    template<int Nshift,typename A,typename E,typename T,int N,
    	template<typename> class R,typename P>
    void write_to_array(A& a,const expr<E,T,N,R,P>& e,false_ /*exit*/);
            
    template<int Nshift,typename A,typename E,typename T,int N,
    	template<typename> class R,typename P>
    void write_to_array(A& a,const expr<E,T,N,R,P>& e,true_ /*exit*/);

    template<int Nshift,typename A,typename E,typename T,int N,
    	template<typename> class R,typename P>
    void write_to_array(A& a,const expr<E,T,N,R,P>& e);

}// auto_size  
}// detail      
}// assign
}// boost

#endif

