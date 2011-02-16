//////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::map_pdf::product_pdf::product_pdf.hpp              //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_PRODUCT_PDF_PRODUCT_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_PRODUCT_PDF_PRODUCT_PDF_HPP_ER_2009
#include <boost/statistics/detail/distribution_common/meta/inherit/policy.hpp>
#include <boost/statistics/detail/distribution_common/meta/inherit/value.hpp>
#include <boost/statistics/detail/distribution_toolkit/meta/is_pseudo_scalar_distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

    // A pseudo distribution resulting from product of the pdf of two
    // distributions
    template<typename A,typename B>
    class product_pdf : public distribution::meta::inherit_policy<
        A,
        distribution::meta::inherit_value<A> 
    >{

        public:

        product_pdf(){}
        explicit product_pdf(
            const A& a,
            const B& b
        ):a_(a),b_(b){}
        
        product_pdf(const product_pdf& that):a_(that.a_),b_(that.b_){}

        product_pdf&
        operator=(const product_pdf& that){
            if(&that!=this){
                a_ = that.a_;
                b_ = that.b_;
            }
            return *this;
        }

        const A& first()const{ return a_; }
        const B& second()const{ return b_; }

        protected:
        A a_;
        B b_;
    };

    template<typename A,typename B>
    product_pdf<A,B>
    make_product_pdf(const A& a,const B& b){ 
        return product_pdf<A,B>(a,b); 
    }

}// distribution
}// toolkit

}// detail
}// statistics
}// boost

#endif
