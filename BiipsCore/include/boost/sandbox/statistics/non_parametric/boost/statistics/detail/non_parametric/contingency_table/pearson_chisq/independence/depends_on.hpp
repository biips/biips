///////////////////////////////////////////////////////////////////////////////
// depends_on.hpp                                                            //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_INDEPENDENCE_DEPENDS_ON_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_INDEPENDENCE_DEPENDS_ON_HPP_ER_2010
#include <boost/mpl/push_back.hpp>
#include <boost/accumulators/statistics/detail/weighted_count.hpp>
#include <boost/accumulators/framework/detail/unpack_depends_on.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/cells/marginal_cells.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace pearson_chisq{
namespace independence_aux{

    template<typename Keys>
    struct depends_on{
        typedef typename cells::marginal_cells<Keys>::type marginal_;
        typedef contingency_table::cells::tag::cells<Keys> joint_;
//        typedef boost::accumulators::tag::count count_;
        typedef boost::accumulators::tag::weighted_count weighted_count_;
        typedef typename boost::mpl::push_back<
            typename boost::mpl::push_back<
                marginal_,
                joint_
            >::type,
            weighted_count_
        >::type dependencies_;
        
        typedef typename boost::accumulators::detail::unpack_depends_on<
            dependencies_
        >::type type;
    };

}// independence_aux
}// pearson_chisq
}// contingency_table
}// detail
}// statistics
}// boost

#endif
