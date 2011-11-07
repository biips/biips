//////////////////////////////////////////////////////////////////////////////
// detail::multi_array::serialization::save.hpp                             //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_MULTI_ARRAY_SERIALIZATION_SAVE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_MULTI_ARRAY_SERIALIZATION_SAVE_HPP_ER_2009
#include <boost/multi_array.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/nvp.hpp>
//#include <boost/archive/text_oarchive.hpp>

namespace boost{ namespace serialization{

    template<typename Archive,typename T>
    inline void save(
        Archive & ar,
        const boost::multi_array<T,2> & t,
        const unsigned int file_version
    )
	{
    	typedef boost::multi_array<T,2> multi_array_;
        typedef typename multi_array_::size_type size_;
    
        size_ n0 = (t.shape()[0]);
        ar << BOOST_SERIALIZATION_NVP(n0);
        size_ n1 = (t.shape()[1]);
        ar << BOOST_SERIALIZATION_NVP(n1);
        ar << boost::serialization::make_array(t.data(), t.num_elements());
    }

} 
}

#endif