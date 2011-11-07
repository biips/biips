///////////////////////////////////////////////////////////////////////////////
// marginal_cells.hpp                                                        //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_CELLS_MARGINAL_CELLS_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_CELLS_MARGINAL_CELLS_HPP_ER_2010
#include <boost/mpl/fold.hpp>
#include <boost/mpl/vector/vector0.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/cells/cells.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace cells{

    template<typename Keys>
    struct marginal_cells : boost::mpl::fold<
        Keys,
        boost::mpl::vector0<>,
        boost::mpl::push_back<
            boost::mpl::_1,
            contingency_table::cells::tag::cells<
                boost::mpl::vector1<boost::mpl::_2>
            >
        >
    >{};

}// cells
}// contingency_table
}// detail
}// statistics
}// boost

#endif

