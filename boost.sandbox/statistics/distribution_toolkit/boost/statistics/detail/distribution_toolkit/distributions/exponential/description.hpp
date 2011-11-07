//////////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::exponential::description.hpp                //
//                                                                                  //
//  (C) Copyright 2009 Erwann Rogard                                                //
//  Use, modification and distribution are subject to the                           //
//  Boost Software License, Version 1.0. (See accompanying file                     //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)                //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_EXPONENTIAL_DESCRIPTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_EXPONENTIAL_DESCRIPTION_HPP_ER_2009
#include <string>
#include <boost/format.hpp>
#include <boost/math/distributions/exponential.hpp>

namespace boost{
namespace math{

    template<typename T,typename P>
    std::string 
    description(
        const boost::math::exponential_distribution<T,P>& dist
    ){
        static const char* msg = "exponential(%1%)";
        boost::format f(msg); f%dist.lambda();
        return f.str();
    }
    
}// math
}// boost

#endif
