//////////////////////////////////////////////////////////////////////////////
// cross_validation::extractor::concept.hpp                                 //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_EXTRACTOR_CONCEPT_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_EXTRACTOR_CONCEPT_HPP_ER_2009
#include <boost/concept_check.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace extractor{

    template<typename F,typename U>
    struct Concept{

        BOOST_CONCEPT_USAGE(Concept){
            f(u);
        }
            
        private:
        F f;
        U u; // data unit
            
    };
    
}// extractor
}// cross_validation
}// detail
}// statistics
}// boost

#endif