/////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::gamma::os.hpp 				   		   //
//                                                                             //
//  (C) Copyright 2009 Erwann Rogard                                           //
//  Use, modification and distribution are subject to the                      //
//  Boost Software License, Version 1.0. (See accompanying file                //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)           //
/////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_GAMMA_OS_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_GAMMA_OS_HPP_ER_2009
#include <boost/statistics/detail/distribution_toolkit/distributions/gamma/description.hpp>

namespace boost{
namespace math{

    template<typename T,typename P>
	std::ostream&
    operator<<(
    	std::ostream& os,
        const boost::math::gamma_distribution<T,P>& dist
    )
    {
        return (os << description(dist));
    }

}// math
}// boost

#endif
