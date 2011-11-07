//////////////////////////////////////////////////////////////////////////////
// cross_validation::extractor::meta_iterator.hpp                           //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_EXTRACTOR_META_RANGE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_EXTRACTOR_META_RANGE_HPP_ER_2009
#include <boost/range.hpp>
#include <boost/statistics/detail/cross_validation/extractor/meta_iterator.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace extractor{
namespace meta{

    template<typename F,typename S>
    struct range{
        typedef typename range_iterator<S>::type seq_iter_;
        typedef typename meta::iterator<F,seq_iter_> meta_;
        typedef typename meta_::type ex_iter_;
        typedef typename iterator_range<ex_iter_>::type type; 

        static type make(const F& f, seq_iter_ b, seq_iter_ e){
            return type(
                meta_::make(f,b),
                meta_::make(f,e)
            );
        }

    };

}// meta
}// extractor
}// cross_validation
}// detail
}// statistics
}// boost

#endif