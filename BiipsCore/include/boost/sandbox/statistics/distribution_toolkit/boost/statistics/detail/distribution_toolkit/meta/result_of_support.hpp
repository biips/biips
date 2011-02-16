//////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::meta::result_of_support.hpp              			    //
//                                                                              //
//  (C) Copyright 2009 Erwann Rogard                                            //
//  Use, modification and distribution are subject to the                       //
//  Boost Software License, Version 1.0. (See accompanying file                 //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)            //
//////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_DISTRIBUTION_TOOLKIT_META_RESULT_OF_SUPPORT_HPP_ER_2009
#define BOOST_DISTRIBUTION_TOOLKIT_META_RESULT_OF_SUPPORT_HPP_ER_2009
#include <utility>
#include <boost/statistics/detail/distribution_common/meta/value.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace common{
namespace meta{

	template<typename D>
    struct result_of_support
    {
    	typedef typename distribution::common::meta::value<D>::type val_;
        typedef std::pair<val_,val_> type;
    }

}// meta
}// toolkit
}// distribution
}// detail
}// statistics
}// boost

#endif
