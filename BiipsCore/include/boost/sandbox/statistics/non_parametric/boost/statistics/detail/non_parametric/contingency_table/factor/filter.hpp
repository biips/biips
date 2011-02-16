///////////////////////////////////////////////////////////////////////////////
// filter.hpp                                                                //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_FILTER_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_FILTER_HPP_ER_2010
#include <boost/mpl/bool.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/factor/levels.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace factor{

    template<typename Keys>
    struct filter{
    
        template<typename Key>
        struct contains : boost::mpl::contains<Keys, Key>{};
  
        template<typename T>
        struct apply : boost::mpl::bool_<false>{};
        
        template<typename Key>
        struct apply< factor::tag::levels<Key> > : contains<Key>{};
    };

}// factor
}// contingency_table
}// detail
}// statistics
}// boost

#endif
