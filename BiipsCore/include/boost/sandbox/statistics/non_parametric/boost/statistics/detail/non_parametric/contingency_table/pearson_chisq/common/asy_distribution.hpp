///////////////////////////////////////////////////////////////////////////////
// asy_distribution.hpp                                                      //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_ASY_DISTRIBUTION_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_ASY_DISTRIBUTION_HPP_ER_2010
#include <boost/math/distributions/chi_squared.hpp>
#include <boost/math/policies/policy.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace pearson_chisq{
namespace result_of{

    template<typename T1,typename Policy = boost::math::policies::policy<> >
    struct asy_distribution{
        typedef boost::math::chi_squared_distribution<T1,Policy> type;
    };

}

    template<typename T1,typename AccSet,typename H0,typename Policy>
    typename pearson_chisq::result_of::asy_distribution<T1,Policy>::type 
    asy_distribution(
        const AccSet& acc,
        const H0& hypothesis,
        const Policy& pol
    )
    {
        typename pearson_chisq::result_of::asy_distribution<
            T1,Policy>::type result_;
        return result_(
            pearson_chisq::df( hypothesis )
        );
    }

    template<typename T1,typename AccSet,typename H0>
    typename pearson_chisq::result_of::asy_distribution<T1>::type 
    asy_distribution(const AccSet& acc,const H0& hypothesis){
        typedef boost::math::policies::policy<> pol_;
        return pearson_chisq::asy_distribution(
            acc,
            hypothesis,
            pol_()
        );
    }

}// pearson_chisq
}// contingency_table
}// detail
}// statistics
}// boost

#endif
