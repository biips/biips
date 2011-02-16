///////////////////////////////////////////////////////////////////////////////
// second_accumulator.hpp                                                    //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_DETAIL_SECOND_ACCUMULATOR_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_DETAIL_SECOND_ACCUMULATOR_HPP_ER_2010
#include <boost/range.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace contingency_table{
namespace impl{

    template<typename T>
    struct second_accumulator
    {
        typedef std::size_t result_type;
        
        second_accumulator(T& init):sum(init){}
        
        template<typename U>
        void operator()(const U& u)const{
            sum += u.second;
        }
                
        mutable T& sum;
    };

    template<typename T>
    second_accumulator<T> make_second_accumulator(T& init){
        return second_accumulator<T>(init);
    }

}// impl
}// contingency_table
}// detail
}// statistics
}// boost

#endif
