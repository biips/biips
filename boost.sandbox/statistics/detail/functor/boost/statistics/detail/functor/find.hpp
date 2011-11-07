///////////////////////////////////////////////////////////////////////////////
// functional::find.hpp                                                      //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_FUNCTIONAL_FIND_HPP_ER_2009
#define BOOST_FUNCTIONAL_FIND_HPP_ER_2009
#include <algorithm>

namespace boost{
namespace functional{

// std::find made into a functor
//
// TODO specialize result_of<In(find)(In,In,const T&)>
struct find{
    find(){}
    
    template<typename In,typename T>
    In operator()(In b,In e,const T& t)const;
};

    
template<typename In,typename T>
In find::operator()(In b,In e,const T& t)const{
    return std::find(
        b,
        e,
        t
    );
}
    
}//functional
}//boost
#endif