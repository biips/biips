//////////////////////////////////////////////////////////////////////////////
// detail::fusion::serialization::vector.hpp                             	//
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_FUSION_SERIALIZATION_VECTOR_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_FUSION_SERIALIZATION_VECTOR_HPP_ER_2009
#include <boost/fusion/include/make_vector.hpp>
#include <boost/statistics/detail/fusion/serialization/save.hpp>
#include <boost/statistics/detail/fusion/serialization/load.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost{ namespace serialization{


	// A generic function restricted by enable_if<is_sequence<S>,void>
    // causes ambiguous compile error. So, apparently, each type of sequence and
    // each length within requires an overoad.
    
    // TODO use a macro
    
	// n = 1

    template<class Archive,typename T1>
    void serialize(
        Archive & ar,
        boost::fusion::vector<T1>& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }

    template<class Archive,typename T1>
    void serialize(
        Archive & ar,
        boost::fusion::vector1<T1>& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }

	// n = 2

    template<class Archive,typename T1,typename T2>
    void serialize(
        Archive & ar,
        boost::fusion::vector<T1,T2>& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }

    template<class Archive,typename T1,typename T2>
    void serialize(
        Archive & ar,
        boost::fusion::vector2<T1,T2>& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }

	// n = 3

    template<class Archive,typename T1,typename T2,typename T3>
    void serialize(
        Archive & ar,
        boost::fusion::vector<T1,T2,T3>& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }

    template<class Archive,typename T1,typename T2,typename T3>
    void serialize(
        Archive & ar,
        boost::fusion::vector3<T1,T2,T3>& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }


}}

#endif