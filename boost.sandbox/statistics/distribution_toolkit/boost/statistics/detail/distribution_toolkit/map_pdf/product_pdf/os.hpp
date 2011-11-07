/////////////////////////////////////////////////////////////////////////////////
// distribution::toolkit::map_pdf::inverse_pdf::os.hpp 				   		   //
//                                                                             //
//  (C) Copyright 2009 Erwann Rogard                                           //
//  Use, modification and distribution are subject to the                      //
//  Boost Software License, Version 1.0. (See accompanying file                //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)           //
/////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_PRODUCT_PDF_OS_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_TOOLKIT_MAP_PDF_PRODUCT_PDF_OS_HPP_ER_2009
#include <boost/statistics/detail/distribution_toolkit/map_pdf/inverse_pdf/inverse_pdf.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace toolkit{

    template<typename F,typename S>
	std::ostream&
    operator<<(
    	std::ostream& os,
        const product_pdf<F,S>& dist
    )
    {
        return (os << description(dist));
    }

}// toolkit
}// distribution
}// detail
}// statistics
}// boost

#endif
