//////////////////////////////////////////////////////////////////////////////
// cross_validation::extractor::identity.hpp                                //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_EXTRACTOR_IDENTITY_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_EXTRACTOR_IDENTITY_HPP_ER_2009

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace extractor{

    struct identity{
    
        identity(){}
    
        template<typename T>
        struct result{};

        template<typename T>
        struct result<identity(const T&)>{
            typedef const T& type;
        };
    
        template<typename T>
        const T&
        operator()(const T& t)const{ return t; }
    
    };
        
}// iterator
}// cross_validation
}// detail
}// statistics
}// boost

#endif