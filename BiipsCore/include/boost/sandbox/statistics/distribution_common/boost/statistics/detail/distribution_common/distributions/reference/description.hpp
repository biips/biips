//////////////////////////////////////////////////////////////////////////////////
// distribution::common::distributions::reference::description.hpp              //
//                                                                              //
//  (C) Copyright 2009 Erwann Rogard                                            //
//  Use, modification and distribution are subject to the                       //
//  Boost Software License, Version 1.0. (See accompanying file                 //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)            //
//////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_DISTRIBUTIONS_REFERENCE_DESCRIPTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_DISTRIBUTIONS_REFERENCE_DESCRIPTION_HPP_ER_2009
#include <string>
#include <boost/format.hpp>
#include <boost/statistics/detail/distribution_common/distributions/reference/wrapper.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{

    template<typename D>
    std::string 
    description(
        const reference_wrapper<D>& dist
    ){
        static const char* msg = "reference_wrapper<%1%>";
        boost::format f(msg); 
        f%description(dist.distribution());
        return f.str();
    }
    
}// distribution
}// detail
}// statistics
}// boost

#endif
