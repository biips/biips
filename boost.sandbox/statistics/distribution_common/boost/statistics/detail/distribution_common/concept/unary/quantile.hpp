//////////////////////////////////////////////////////////////////////////////////
// distribution::common::concept::concept::unary::quantile.hpp                  //
//                                                                              //
//  (C) Copyright 2009 Erwann Rogard                                            //
//  Use, modification and distribution are subject to the                       //
//  Boost Software License, Version 1.0. (See accompanying file                 //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)            //
//////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_CONCEPT_UNARY_QUANTILE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_CONCEPT_UNARY_QUANTILE_HPP_ER_2009
#include <boost/concept_check.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace concept{

    template<typename B>
    class Quantile : public B{
        typedef typename B::distribution_type   d_;
        typedef typename B::variable_type       x_;
        typedef typename B::probability_type    p_;

        public:
    
        BOOST_CONCEPT_USAGE(Quantile){ x = quantile(d,p); }

        private:
        d_ d;
        x_ x;
        p_ p;
        
    };

}// concept
}// distribution
}// statistics
}// detail
}// boost

#endif
