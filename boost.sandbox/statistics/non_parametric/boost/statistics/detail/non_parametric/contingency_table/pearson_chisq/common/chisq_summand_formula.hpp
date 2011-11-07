///////////////////////////////////////////////////////////////////////////////
// chisq_summand.hpp                                                         //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_CHISQ_SUMMAND_FORMULA_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_CHISQ_SUMMAND_FORMULA_HPP_ER_2010
#include <boost/numeric/conversion/converter.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace pearson_chisq{

    template<typename T1,typename T2,typename T3>
    T1 chisq_summand_formula(const T2& expected_n, const T3& observed_n)
    {
        typedef boost::numeric::converter<T1,T2> conv2_;
        typedef boost::numeric::converter<T1,T3> conv3_;
        T1 summand = conv2_::convert( expected_n ) 
            - conv3_::convert( observed_n );
        summand *= summand;
        return summand / conv2_::convert( expected_n );
    }
    
}// pearson_chisq
}// contingency_table
}// detail
}// statistics
}// boost

#endif
