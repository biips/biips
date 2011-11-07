//////////////////////////////////////////////////////////////////////////////
// assign::detail::range::hold_previous.hpp                                 //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_RANGE_HOLD_PREVIOUS_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_RANGE_HOLD_PREVIOUS_ER_2010_HPP
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/if.hpp>

namespace boost{
namespace assign{
namespace detail{

    template<typename E,bool is_first>
    class hold_previous{
        typedef typename boost::mpl::if_c<is_first,E,E&>::type previous_; 
        
        public:
        
        hold_previous(){} 
        hold_previous(E& p)
            :previous(p){} 
        
        mutable previous_ previous;
    };

}// detail
}// assign
}// boost

#endif