//////////////////////////////////////////////////////////////////////////////
// assign::detail::array::ref.hpp                                           //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_AUTO_SIZE_DETAIL_ARRAY_REF_ER_2010_HPP
#define BOOST_ASSIGN_AUTO_SIZE_DETAIL_ARRAY_REF_ER_2010_HPP
#include <boost/array.hpp>

namespace boost{
namespace assign{
namespace detail{
namespace auto_size{
                   
    template<typename T,int N,template<typename> class R>
    struct ref_array{
    	typedef typename R<T>::type ref_;
        typedef boost::array<ref_,N> type;
    };

}// auto_size  
}// detail      
}// assign
}// boost

#endif

