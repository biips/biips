//////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::map_pdf::inverse_pdf::inverse_pdf.hp               //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_INVERSE_PDF_INVERSE_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_INVERSE_PDF_INVERSE_PDF_HPP_ER_2009
#include <boost/statistics/detail/distribution_common/meta/inherit/policy.hpp>
#include <boost/statistics/detail/distribution_common/meta/inherit/value.hpp>
#include <boost/statistics/detail/distribution_toolkit/meta/is_pseudo_scalar_distribution.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

    // A pseudo distribution resulting from mapping the pdf of a distribution
    // to its inverse
    template<typename D>
    class inverse_pdf : public
        distribution::meta::inherit_policy<
            D,
            distribution::meta::inherit_value<D>
        >
    {
    
    public:
        typedef typename distribution::meta::value<D>::type value_type;

        inverse_pdf(){}
        explicit inverse_pdf(const D& d):d_(d){}
        inverse_pdf(const inverse_pdf& that):d_(that.d_){}
        inverse_pdf&
        operator=(const inverse_pdf& that){
            if(&that!=this){
                d_ = that.d_;
            }
            return *this;
        }

        const D&
        distribution()const{ return d_; }

    private:
        D d_;
    };

    template<typename D>
    inverse_pdf<D>
    make_inverse_pdf(const D& d){ return inverse_pdf<D>(d); }


}// distribution
}// toolkit

}// detail
}// statistics
}// boost


#endif 
