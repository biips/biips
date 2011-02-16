///////////////////////////////////////////////////////////////////////////////
// ars::error.hpp                                                            //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_ERROR_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_ERROR_HPP_ER_2009
#include <stdexcept>
#include <sstream>
#include <boost/format.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{

        class exception : public std::runtime_error{
            typedef std::string str_t;
            public:
            typedef std::runtime_error super_t;
            exception(
                const str_t& a,
                const str_t& b,
                const str_t& c
            ):super_t(this->str(a,b,c)){}

            template<typename T>
            exception(
                const str_t& a,
                const str_t& b,
                const T& c
            ):super_t(this->str(a,b,c)){}

            template<typename T,typename U>
            exception(
                const str_t& a,
                const str_t& b,
                const T& c,
                const U& d
            ):super_t(this->str(a,b,c,d)){}

            private:
            static str_t str(
                const str_t& a,
                const str_t& b,
                const str_t& c
            ){
                boost::format f("Call : %1%, error: %2%, state : %3%");
                f % a % b % c;
                return f.str();
            }
            template<typename T>
            static str_t str(
                const str_t& a,
                const str_t& b,
                const T& c
            ){
                std::ostringstream os;
                os << c;
                return str(a, b, os.str());
            }

            template<typename T,typename U>
            static str_t str(
                const str_t& a,
                const str_t& b,
                const T& c,
                const U& d
            ){
                std::ostringstream os;
                os << c << d;
                return str(a, b, os.str());
            }

        };

}// ars
}// detail
}// statistics
}// boost


#endif // BOOST_STATISTICS_DETAIL_ARS_ERROR_HPP_ER_2009
