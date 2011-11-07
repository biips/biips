///////////////////////////////////////////////////////////////////////////////
// iterator::range_cycle.hpp                                                 //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ITERATOR_RANGE_CYCLE_ER_2009
#define BOOST_ITERATOR_RANGE_CYCLE_ER_2009
#include <boost/range.hpp>
#include <boost/utility.hpp> 
#include <boost/iterator/cycle_iterator_ext.hpp> 
#include <boost/iterator/cycle_iterator2_ext.hpp> 
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/iterator/iterator_traits.hpp>

namespace boost{

    // W = cycle_iterator_ext or  = cycle_iterator2_ext
    template<template<typename,typename> class W>
    class iterator_cycle{ 
        public:
        typedef std::size_t                 size_type;
 
        template<typename It>
        struct apply{
            typedef W<It,size_type>         it_;
            typedef iterator_range<it_>     type;
        };
                                            
        template<typename It>
        typename apply<It>::type
        static make(
            It b, 
            It e,
            size_type offset ,
            size_type n
        ){
            typedef typename apply<It>::it_     it_;
            typedef typename apply<It>::type    res_;
            it_ it(b,e,offset);
            return res_(
                it,
                boost::next(it,n)
            );
        }

    };

    // From a range to a cycle
    template<template<typename,typename> class W = cycle_iterator_ext>
    class range_cycle{ 
        public:

        typedef iterator_cycle<W>                       it_cycle_;
        typedef typename it_cycle_::size_type           size_type;
 
        template<typename R>
        struct apply : 
            it_cycle_::template apply<typename range_iterator<R>::type> {};
                                            
        template<typename R>
        typename apply<R>::type
        static make(
            R& r,
            size_type offset,
            size_type n
        ){
            return it_cycle_::make(
                boost::begin(r),
                boost::end(r),
                offset,
                n
            );
        }

    };
    
} //boost

#endif