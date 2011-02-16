//////////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::poisson::description.hpp                	//
//                                                                                  //
//  (C) Copyright 2009 Erwann Rogard                                                //
//  Use, modification and distribution are subject to the                           //
//  Boost Software License, Version 1.0. (See accompanying file                     //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)                //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_POISSON_DESCRIPTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_POISSON_DESCRIPTION_HPP_ER_2009
#include <ostream>
#include <boost/format.hpp>
#include <boost/math/distributions/poisson.hpp>

namespace boost{
namespace math{

    template<typename T,typename P>
    std::string
    description(const boost::math::poisson_distribution<T,P>& dist)
    {
        static const char* msg = "poisson(%1%)";
        format f(msg); f % dist.mean();
        return f.str();
    }
    
}// math
}// boost


#endif