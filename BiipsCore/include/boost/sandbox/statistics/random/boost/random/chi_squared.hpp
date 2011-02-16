//////////////////////////////////////////////////////////////////////////////
// random::chi_squared.hpp                                                  //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_RANDOM_CHI_SQUARED_HPP_ER_2009
#define BOOST_RANDOM_CHI_SQUARED_HPP_ER_2009
#include <vector>
#include <algorithm>
#include <iterator>
#include <boost/range.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/chi_squared.hpp>
#include <boost/math/distributions/chi_squared.hpp>

namespace boost{
namespace random{

    template<typename T>
    class chi_squared_distribution{
            typedef boost::normal_distribution<T> nd_t;
        public:
            typedef typename nd_t::input_type input_type;
            typedef typename nd_t::result_type result_type;

        chi_squared_distribution(): df_(2) {}
        chi_squared_distribution(unsigned df): df_(df) {}
        chi_squared_distribution(const chi_squared_distribution& that)
        :df_(that.df_){}
        chi_squared_distribution&
        operator=(const chi_squared_distribution& that){
            if(&that!=this){
                df_ = that.df_;
            }
            return *this;
        }

        template<typename U>
        result_type
        operator()(U& urng){
            typedef boost::variate_generator<U&,nd_t> vg_t;
            static nd_t nd(0,1);
            result_type z;
            result_type res = static_cast<T>(0);
            for(unsigned i = 0; i<this->df(); i++){
                z = nd(urng);
                res += (z * z);
            }
            return res;
        }

        result_type min () const { return static_cast<result_type>(0); }
        result_type max () const { return (nd_t().max)(); }
        unsigned df()const { return df_; }
        
        typedef math::chi_squared_distribution<T> math_dist_;
        
        operator math_dist_ (){ return math_dist(this->df()); }
                
        private:
            unsigned df_;
    };

}
}


#endif // CHI_SQUARE_HPP_INCLUDED
