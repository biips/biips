/////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::location_scale::os.hpp 			   //
//                                                                             //
//  (C) Copyright 2009 Erwann Rogard                                           //
//  Use, modification and distribution are subject to the                      //
//  Boost Software License, Version 1.0. (See accompanying file                //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)           //
/////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_OS_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_LOCATION_SCALE_OS_HPP_ER_2009
#include <boost/statistics/detail/distribution_toolkit/distributions/location_scale/description.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

    template<typename D>
	std::ostream&
    operator<<(
    	std::ostream& os,
        const location_scale_distribution<D>& dist
    )
    {
        return (os << description(dist));
    }

}// toolkit
}// distribution
}// detail
}// statistics
}// boost

#endif
