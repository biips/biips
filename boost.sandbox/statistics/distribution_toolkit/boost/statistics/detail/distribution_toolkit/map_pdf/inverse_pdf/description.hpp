//////////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::distributions::inverse_pdf::description.hpp               //
//                                                                                  //
//  (C) Copyright 2009 Erwann Rogard                                                //
//  Use, modification and distribution are subject to the                           //
//  Boost Software License, Version 1.0. (See accompanying file                     //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)                //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_INVERSE_PDF_DESCRIPTION_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_INVERSE_PDF_DESCRIPTION_HPP_ER_2009
#include <string>
#include <boost/format.hpp>
#include <boost/statistics/detail/distribution_toolkit/map_pdf/inverse_pdf/inverse_pdf.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

    template<typename D>
    std::string
    description(const inverse_pdf<D>& dist)
    {
        static const char* msg = "inverse_pdf<%1%>";
        format f(msg); f%description(dist.distribution());
        return (f.str());
    }
    
}// toolkit
}// distribution
}// detail
}// statistics
}// boost

#endif
