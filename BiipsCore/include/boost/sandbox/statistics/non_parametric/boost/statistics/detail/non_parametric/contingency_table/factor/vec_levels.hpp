///////////////////////////////////////////////////////////////////////////////
// vec_levels.hpp                                                            //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_VEC_LEVELS_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_VEC_LEVELS_HPP_ER_2010
#include <boost/mpl/fold.hpp>
#include <boost/mpl/vector/vector0.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/factor/levels.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace factor{
   
   template<typename Keys>
    struct vec_levels : boost::mpl::fold< 
        Keys,
        boost::mpl::vector0<>,
        boost::mpl::push_back<
            boost::mpl::_1,
            contingency_table::factor::tag::levels<boost::mpl::_2>
        >
    >{};
   
   
}// factor
}// contingency_table
}// detail
}// statistics
}// boost

#endif
