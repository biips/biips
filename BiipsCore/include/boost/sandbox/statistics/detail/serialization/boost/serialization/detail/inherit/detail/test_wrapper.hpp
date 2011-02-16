//////////////////////////////////////////////////////////////////////////////
// serialization::detail::inherit::detail::test_wrapper.hpp                 //
//                                                                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_SERIALIZATION_DETAIL_INHERIT_DETAIL_TEST_WRAPPER_HPP_ER_2010
#define BOOST_SERIALIZATION_DETAIL_INHERIT_DETAIL_TEST_WRAPPER_HPP_ER_2010
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace boost{
namespace serialization{
namespace detail{

	template<typename T>
    struct test_wrapper{
    	typedef boost::accumulators::dont_care dont_care_;

		test_wrapper(){}
    	test_wrapper(dont_care_){}

		T value;
        
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & value;
        }
    
    };

    template<typename A,typename B,typename T>
	std::basic_ostream<A,B>&
    operator<<(std::basic_ostream<A,B>& os,const test_wrapper<T>& that){
		return os << that.value;
    }


}// detail
}// serialization
}// boost

#endif