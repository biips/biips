///////////////////////////////////////////////////////////////////////////////
// domain_error_logger.hpp                                                   //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_DOMAIN_ERROR_LOGGER_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_DOMAIN_ERROR_LOGGER_HPP_ER_2010
#include <string>
#include <boost/format.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace factor{

class domain_error_logger{
   typedef std::string str_;
   typedef boost::format f_;

    public:
   
    domain_error_logger():is_error_(false){}

    // Call add<Key,T>(data), not add<Key>(data) (strings would otherwise get 
    // converted to const char* or smthg like that).
    template<typename Key,typename T>
    void add(const T& data)const{
        static const str_ str ="(%1%,%2%)"; 
        if(this->is_error()){ 
            this->log_ += ","; 
        }else{ 
            this->is_error_ = true; 
        }
        this->log_ += ( f_(str) % typeid(T).name() % data ).str(); 
    }

    void reset(){ this->is_error_ = false; }

    bool is_error()const{ return this->is_error_; }
    str_ operator()()const{ 
       static const str_ pre = "(key,level) = { ";
       static const str_ suf = " }";
       return pre + this->log_ + suf; 
    }

    private:
    mutable bool is_error_;
    mutable str_ log_;
   
};

}// factor
}// contingency_table
}// detail
}// statistics
}// boost

#endif
