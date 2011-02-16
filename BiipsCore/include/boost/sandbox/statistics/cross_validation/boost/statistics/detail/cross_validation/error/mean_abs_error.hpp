//////////////////////////////////////////////////////////////////////////////
// cross_validation::error::mean_abs_error.hpp                              //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ERROR_MEAN_ABS_ERROR_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ERROR_MEAN_ABS_ERROR_HPP_ER_2009
#include <cmath>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/range.hpp>
#include <boost/vector_space/functional/l1_distance.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace error{

    template<typename It,typename It1>
    typename iterator_value<It>::type
    mean_abs_error(
        It b,
        It e,
        It1 b1
    ){

        typedef iterator_range<It>                          range_;
        typedef typename iterator_difference<It>::type      diff_;
        typedef iterator_range<It1>                         range1_;
        typedef typename iterator_value<It>::type           val_;
        typedef vector_space::template l1_distance<range_>   l1_;
        
        diff_ d = std::distance(b,e);
        
        l1_ l1(range_(b,e));
        range1_ range1(
            b1,
            boost::next(
                b1,
                d
            )
        );
        val_ res = l1(range1);
        res /= static_cast<val_>(d);
        res = sqrt( res );
        return res;
    };
    
}// error
}// cross_validation
}// detail
}// statistics
}// boost

#endif