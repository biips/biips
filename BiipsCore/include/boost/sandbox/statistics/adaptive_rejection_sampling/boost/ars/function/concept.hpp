///////////////////////////////////////////////////////////////////////////////
// ars::function::concept.hpp                                                //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_FUNCTION_CONCEPT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_FUNCTION_CONCEPT_HPP_ER_2009
#include <boost/concept_check.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{
namespace function{

template<typename F,typename T>
class concept{

    BOOST_CONCEPT_USAGE((
        f(x,log_pdf,dlog_pdf)
    ));

    private:
        F f;
        T x;
        T log_pdf;
        T dlog_pdf;

};

}
}// ars
}// detail
}// statistics
}// boost

#endif