//////////////////////////////////////////////////////////////////////////////
// random::students_t_distribution.hpp                                      //
//                                                                          //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_RANDOM_STUDENTS_T_HPP_ER_2009
#define BOOST_RANDOM_STUDENTS_T_HPP_ER_2009
#include <vector>
#include <algorithm>
#include <iterator>
#include <boost/range.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/chi_squared.hpp>

namespace boost{
namespace random{

    // Samples from a students_t distribution
    template<typename T>
    class students_t_distribution{
            typedef boost::normal_distribution<T>           nd_;
            typedef random::chi_squared_distribution<T>     cs_;
        public:
            typedef typename nd_::input_type input_type;
            typedef typename nd_::result_type result_type;

        students_t_distribution():df_(2){}
        students_t_distribution(unsigned df):df_(df){}

        template<typename U>
        result_type
        operator()(U& urng){
            static nd_ nd(0,1);
            static cs_ cs(df_);
            result_type z = nd(urng);
            result_type d = cs(urng);
            d /= static_cast<result_type>(df_);
            d = sqrt(d);
            return z / d;
        }

        unsigned df()const{ return df_; }

        private:
            unsigned df_;
    };


}// random
}// boost

#endif
