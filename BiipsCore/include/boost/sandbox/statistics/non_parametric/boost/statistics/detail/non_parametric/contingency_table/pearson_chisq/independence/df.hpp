///////////////////////////////////////////////////////////////////////////////
// df.hpp                                                                    //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_INDEPENDENCE_DF_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_INDEPENDENCE_DF_HPP_ER_2010
#include <boost/statistics/detail/non_parametric/contingency_table/cells/cells_count.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/pearson_chisq/common/df_formula.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/pearson_chisq/independence/lost_df.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace pearson_chisq{

  	template<typename Keys,typename AccSet>
    long df(AccSet const& acc,tag::independence<Keys> hypothesis){
        namespace ns = contingency_table::cells;
        return pearson_chisq::df_formula<long>(
            ns::cells_count<Keys>( acc ),
            lost_df( acc, hypothesis )
        );
    }

}// pearson_chisq
}// contingency_table
}// detail
}// statistics
}// boost

#endif

