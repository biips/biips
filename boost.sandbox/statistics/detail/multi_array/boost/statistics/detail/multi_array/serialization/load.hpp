//////////////////////////////////////////////////////////////////////////////
// detail::multi_array::serialization::load.hpp                             //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_MULTI_ARRAY_SERIALIZATION_LOAD_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_MULTI_ARRAY_SERIALIZATION_LOAD_HPP_ER_2009
#include <boost/multi_array.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/nvp.hpp>
//#include <boost/archive/text_iarchive.hpp>

namespace boost{ namespace serialization{

    template<class Archive,class T>
    inline void load(
        Archive & ar,
        boost::multi_array<T,2> & t,
        const unsigned int file_version
    )
    {
		typedef boost::multi_array<T,2> multi_array_;
		typedef typename multi_array_::size_type size_;
        
        size_ n0;
        ar >> BOOST_SERIALIZATION_NVP(n0);
        size_ n1;
        ar >> BOOST_SERIALIZATION_NVP(n1);
            
        t.resize(boost::extents[n0][n1]);
        ar >> make_array(t.data(), t.num_elements());
    }
}}

#endif