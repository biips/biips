///////////////////////////////////////////////////////////////////////////////
// vector_space::data::detail::cref.hpp                                      //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
/////////////////////////////////////////////////////////////////////////////// 
#ifndef BOOST_STATISTICS_DETAIL_VECTOR_SPACE_DATA_DETAIL_CREF_H_ER_2009
#define BOOST_STATISTICS_DETAIL_VECTOR_SPACE_DATA_DETAIL_CREF_H_ER_2009
#include <boost/mpl/not.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/range.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace vector_space{
namespace detail{

// A data structure that keeps a const reference to a range. 
template<typename X>
struct cref{
    typedef is_reference<X> is_ref_x_;
    typedef is_const<X> is_const_x_;
    BOOST_MPL_ASSERT((boost::mpl::not_<is_ref_x_>));
    BOOST_MPL_ASSERT((boost::mpl::not_<is_const_x_>));
    
    cref(const X& x):x_(x){}
    cref(const cref& that):x_(that.x_){}
    typedef typename range_iterator<const X>::type const_iterator;
    
    const_iterator begin()const{ return boost::begin(x_); }
    const_iterator end()const{ return boost::end(x_); }
    
    const X& x_;
};

}// details
}// vector_space
}// detail
}// statistics
}// boost

#endif