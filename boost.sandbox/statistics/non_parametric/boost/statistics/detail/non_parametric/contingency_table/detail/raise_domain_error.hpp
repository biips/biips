///////////////////////////////////////////////////////////////////////////////
// raise_domain_error.hpp                                                    //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_DETAIL_RAISE_DOMAIN_ERROR_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_DETAIL_RAISE_DOMAIN_ERROR_HPP_ER_2010
#include <boost/math/policies/error_handling.hpp>  
#include <boost/math/policies/policy.hpp>  

// Unlike their boost::math::policies counterparts, these error policy actions
// do not return a value bec. quiet_NaN etc. may not be defined for the 

namespace boost{
namespace statistics{
namespace detail{
namespace contingency_table{

// fwd_declare
template <class T> 
T user_domain_error(const char* function, const char* message);

namespace detail{

template <class T>
inline void raise_domain_error(
    const char* function, 
    const char* message, 
    const ::boost::math::policies::domain_error< 
        ::boost::math::policies::throw_on_error>&)
{
   namespace ns = boost::math::policies::detail;
   ns::raise_error<std::domain_error, T>(function, message);
}

template <class T>
inline void raise_domain_error(
    const char* , 
    const char* , 
    const ::boost::math::policies::domain_error< 
        ::boost::math::policies::ignore_error>&)
{}

template <class T>
inline void raise_domain_error(
    const char* , 
    const char* , 
    const ::boost::math::policies::domain_error< 
        ::boost::math::policies::errno_on_error>&)
{
   errno = EDOM;
}

template <class T>
inline void raise_domain_error(
    const char* function, 
    const char* message, 
    const  ::boost::math::policies::domain_error< 
        ::boost::math::policies::user_error>&)
{
   contingency_table::user_domain_error<T>(function, message); 
}

}// detail

template <class T, class Policy>
inline void raise_domain_error(
    const char* function, 
    const char* message, 
    const Policy& p
)
{
   typedef typename Policy::domain_error_type policy_type;
   contingency_table::detail::raise_domain_error<T>(
      function, message ? message : "Domain Error evaluating function", 
      policy_type());
}

}// contingency_table
}// detail
}// statistics
}// boost

#endif
