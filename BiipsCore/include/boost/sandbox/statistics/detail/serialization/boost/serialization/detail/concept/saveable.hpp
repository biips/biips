//////////////////////////////////////////////////////////////////////////////
// serialization::detail::concept::saveable.hpp         					//
//                                                                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_SERIALIZATION_DETAIL_CONCEPT_SAVEABLE_HPP_ER_2010
#define BOOST_SERIALIZATION_DETAIL_CONCEPT_SAVEABLE_HPP_ER_2010
#include <boost/concept_check.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace boost{
namespace serialization{
namespace detail{
namespace concept{

    template<typename T>
    class Saveable{
    	typedef boost::archive::text_oarchive oa_;

        public:
    
        BOOST_CONCEPT_USAGE(Saveable){ 
            oa << t;		
        }

        private:
		T t;
        oa_ oa;
    };
    
}// concept
}// detail
}// serialization
}// boost

#endif

