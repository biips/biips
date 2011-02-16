//////////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::chi_squared::description.hpp                //
//                                                                                  //
//  (C) Copyright 2009 Erwann Rogard                                                //
//  Use, modification and distribution are subject to the                           //
//  Boost Software License, Version 1.0. (See accompanying file                     //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)                //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_CHI_SQUARED_DESCRIPTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_CHI_SQUARED_DESCRIPTION_HPP_ER_2009
#include <ostream>
#include <boost/format.hpp>
#include <boost/math/distributions/chi_squared.hpp>

namespace boost{
namespace math{

    template<typename T,typename P>
    std::string
    description(const boost::math::chi_squared_distribution<T,P>& dist)
    {
        static const char* msg = "chi_squared(%1%)";
        format f(msg); f % dist.degrees_of_freedom();
        return f.str();
    }
    
}// math
}// boost


#endif
