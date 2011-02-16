//////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::map_pdf::ratio_pdf::ratio_pdf.hpp                  //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_RATIO_PDF_RATIO_PDF_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_RATIO_PDF_RATIO_PDF_HPP_ER_2009
#include <boost/call_traits.hpp>
#include <boost/statistics/detail/distribution_toolkit/map_pdf/inverse_pdf/inverse_pdf.hpp>
#include <boost/statistics/detail/distribution_toolkit/map_pdf/product_pdf/product_pdf.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{


    template<typename A,typename B>
    struct meta_ratio_pdf{
        typedef inverse_pdf<B> inv_;
        typedef product_pdf<A, inv_ > type;
        static type call(const A& a,const B& b){
            return type(
                a,
                inv_(b)
            );
         } 
    };

    template<typename A,typename B>
    typename meta_ratio_pdf<A,B>::type
    make_ratio_pdf(const A& a, const B& b)
    {
        typedef meta_ratio_pdf<A,B> meta_;
        return meta_::call(a,b);
    }

}// distribution
}// toolkit
}// detail
}// statistics
}// boost

#endif
