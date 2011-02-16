///////////////////////////////////////////////////////////////////////////////
// tag.hpp                                                                   //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_INDEPENDENCE_TAG_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_INDEPENDENCE_TAG_HPP_ER_2010

#include <boost/mpl/always.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/pearson_chisq/independence/impl.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/pearson_chisq/independence/depends_on.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace pearson_chisq{
namespace tag
{
    template<typename Keys>
    struct independence
      : independence_aux::template depends_on<Keys>::type
    {
        typedef boost::mpl::always< 
            pearson_chisq::impl::independence<Keys> 
        > impl;
    };
    
}// tag
}// pearson_chisq
}// contingency_table
}// detail
}// statistics
}// boost

#endif

