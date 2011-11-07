///////////////////////////////////////////////////////////////////////////////
// check_against_domain.hpp                                                  //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_CHECK_AGAINST_DOMAIN_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_CHECK_AGAINST_DOMAIN_HPP_ER_2010
#include <boost/fusion/include/at_key.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/factor/levels.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace factor{

    template<typename A,typename E>
    struct check_against_domain{
    
        typedef void result_type; 

        check_against_domain(const A& acc, const E& error_logger)
            :acc_(acc),error_logger_(error_logger){}
                 
        template<typename T>
        void operator()(const T& pair)const
        {
            typedef typename T::first_type key_;
            typedef typename T::second_type data_;
            namespace ns = contingency_table::factor;
            
            // See domain_error_logger for justification of fully qualified.
            this->impl<key_,data_>(
                pair.second,
                ns::extract::levels<key_>( this->acc_ )
            );
        }
     
        private:
        template<typename Key,typename T,typename L>
        void impl(const T& data,const L& levels)const{
            if( levels.count( data ) != 1 )
            {
                this->error_logger_.template add<Key,T>( data );
            }  
        }
        mutable const A& acc_;
        mutable const E& error_logger_;
    };

template<typename A,typename E>
contingency_table::factor::check_against_domain<A,E>
make_check_against_domain(const A& acc,const E& error_logger){
    typedef contingency_table::factor::check_against_domain<A,E> checker_;
    return checker_(acc,error_logger);
}

}// factor
}// contingency_table
}// detail
}// statistics
}// boost

#endif
