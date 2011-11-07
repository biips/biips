///////////////////////////////////////////////////////////////////////////////
// df_formula.hpp                                                            //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_DF_FORMULA_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_PEARSON_CHISQ_DF_FORMULA_HPP_ER_2010

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace pearson_chisq{

// http://wapedia.mobi/en/Pearson%27s_chi-square_test

// General formula
//  df = ( N1 - 1 ) - N2 
//  N1 = # cells in table
//  N2 = # of estimated probabilities under H0 a.k.a lost df under H0
//
// For example, test of independence in dimension = 2
// Known result :  df = (r − 1)(c − 1) = rc -r -c + 1
// By formula   :  df = ( rc - 1 ) - ( (r-1) + (c-1) ) = rc -r -c + 1

    template<typename Int>
    Int df_formula(Int number_of_cells,Int lost_df)
    {
        return (number_of_cells - 1) - lost_df;
    }

}// pearson_chisq
}// contingency_table
}// detail
}// statistics
}// boost

#endif
