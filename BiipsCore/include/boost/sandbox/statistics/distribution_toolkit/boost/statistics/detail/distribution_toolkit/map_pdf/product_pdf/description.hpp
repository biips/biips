//////////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::product_pdf::description.hpp               //
//                                                                                  //
//  (C) Copyright 2009 Erwann Rogard                                                //
//  Use, modification and distribution are subject to the                           //
//  Boost Software License, Version 1.0. (See accompanying file                     //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)                //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_PRODUCT_PDF_DESCRIPTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_PRODUCT_PDF_DESCRIPTION_HPP_ER_2009
#include <string>
#include <boost/format.hpp>
#include <boost/statistics/detail/distribution_toolkit/map_pdf/product_pdf/product_pdf.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

    template<typename F,typename S>
    std::string
    description(const product_pdf<F,S>& dist)
    {
        static const char* msg = "product_pdf<%1%,%2%>";
        format f(msg); f%description(dist.first())%description(dist.second());
        return (f.str());
    }
    
}// toolkit
}// distribution
}// detail
}// statistics
}// boost

#endif
