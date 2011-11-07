//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_constants.hpp                           //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_CONSTANTS_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_CONSTANTS_ER_2010_HPP
#include <boost/next_prior.hpp>
#include <boost/range.hpp>
#include <boost/array.hpp>

namespace boost{                                                               
namespace assign{                                                              
namespace detail{                                                              
namespace auto_size{                                                           

namespace check_constants{
     
    const int a = 1;    // 1
    const int b = 5;    // 5
    const int c = 3;    // 3
    const int d = 4;    // 4
    const int e = 2;    // 2
    const int f = 9;    // 7
    const int g = 0;    // 0
    const int h = 7;    // 6

    // array    
    boost::array<int,8> array()
    {                                              
        typedef boost::array<int,8> ar_;
        ar_ ar;
        ar[0] = a; ar[1] = b; ar[2] = c;                                           
        ar[3] = d; ar[4] = e; ar[5] = f;                                           
        ar[6] = g; ar[7] = h;                                                      
        return ar;
    }    
    
    // values 

    template<typename T>
    void check_values_impl(
        const T& a1,const T& b1,const T& c1,const T& d1,
        const T& e1,const T& f1,const T& g1,const T& h1
    )
    {                                                                              
        using namespace check_constants;                                           
        BOOST_ASSIGN_CHECK_EQUAL(a , a1 );                                         
        BOOST_ASSIGN_CHECK_EQUAL(b , b1 );                                          
        BOOST_ASSIGN_CHECK_EQUAL(c , c1 );                                          
        BOOST_ASSIGN_CHECK_EQUAL(d , d1 );                                          
        BOOST_ASSIGN_CHECK_EQUAL(e , e1 );                                          
        BOOST_ASSIGN_CHECK_EQUAL(f , f1 );                                          
        BOOST_ASSIGN_CHECK_EQUAL(g , g1 );                                          
        BOOST_ASSIGN_CHECK_EQUAL(h , h1 );                                          
    }                                                                  

    template<typename T>
    void check_sorted_values_impl(
        const T& a1,const T& b1,const T& c1,const T& d1,
        const T& e1,const T& f1,const T& g1,const T& h1
    )
    {                                                                              
        using namespace check_constants;                                           
        BOOST_ASSIGN_CHECK_EQUAL(a1, g);                           
        BOOST_ASSIGN_CHECK_EQUAL(b1, a);                           
        BOOST_ASSIGN_CHECK_EQUAL(c1, e);                           
        BOOST_ASSIGN_CHECK_EQUAL(d1, c);                           
        BOOST_ASSIGN_CHECK_EQUAL(e1, d);                           
        BOOST_ASSIGN_CHECK_EQUAL(f1, b);                           
        BOOST_ASSIGN_CHECK_EQUAL(g1, h);                           
        BOOST_ASSIGN_CHECK_EQUAL(h1, f);                           
    }                                                                  

    // iterators
    
    template<typename It>
    void check_iterator_impl(It it)
    {                                                                              
        using namespace check_constants;                                           
        check_values_impl(
            *boost::next(it,0),
            *boost::next(it,1),
            *boost::next(it,2),
            *boost::next(it,3),
            *boost::next(it,4),
            *boost::next(it,5),
            *boost::next(it,6),
            *boost::next(it,7)
        );
    }                                                                              

    template<typename It>
    void check_iterator_sorted_impl(It it)
    {                                                                              
        using namespace check_constants;                                           
        check_sorted_values_impl(
            *boost::next(it,0),
            *boost::next(it,1),
            *boost::next(it,2),
            *boost::next(it,3),
            *boost::next(it,4),
            *boost::next(it,5),
            *boost::next(it,6),
            *boost::next(it,7)
        );
    }                                                                              

    // array

    template<typename A>
    void check_array_impl(const A& ar)
    {                                                                              
        BOOST_ASSIGN_CHECK_EQUAL( ar.empty() , false );
        using namespace check_constants;
        check_values_impl(
           ar[0],ar[1],ar[2],ar[3],
           ar[4],ar[5],ar[6],ar[7]
        );                           
    }                                           

}// check_constants
}// auto_size
}// detail
}// assign
}// boost    
    
#endif
