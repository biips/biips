//////////////////////////////////////////////////////////////////////////////
// detail::fusion::serialization::map.hpp                             		//
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_FUSION_SERIALIZATION_MAP_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_FUSION_SERIALIZATION_MAP_HPP_ER_2009
#include <boost/fusion/include/make_map.hpp>
#include <boost/statistics/detail/fusion/serialization/save.hpp>
#include <boost/statistics/detail/fusion/serialization/load.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost{ namespace serialization{

	// A generic function restricted by enable_if<is_sequence<S>,void>
    // causes ambiguous compile error. So, apparently, each type of sequence and
    // each length within requires an overoad.

	// n = 1

    template<class Archive,typename K1,typename D1>
    void serialize(
        Archive & ar,
        boost::fusion::map<boost::fusion::pair<K1,D1> >& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version); 
    }

	// n = 2

    template<class Archive,typename K1,typename K2,typename D1,typename D2>
    void serialize(
        Archive & ar,
        boost::fusion::map<
        	boost::fusion::pair<K1,D1>, 
        	boost::fusion::pair<K2,D2> 
        >& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }

	// n = 3

    template<class Archive,
    	typename K1,typename K2,typename K3,
        typename D1,typename D2,typename D3
    >
    void serialize(
        Archive & ar,
        boost::fusion::map<
        	boost::fusion::pair<K1,D1>, 
        	boost::fusion::pair<K2,D2>, 
        	boost::fusion::pair<K3,D3>
        >& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }

	// n = 4

    template<class Archive,
    typename K1,typename K2,typename K3,typename K4,
    typename D1,typename D2,typename D3,typename D4
    >
    void serialize(
        Archive & ar,
        boost::fusion::map<
            boost::fusion::pair<K1,D1>, 
            boost::fusion::pair<K2,D2>, 
            boost::fusion::pair<K3,D3>,
            boost::fusion::pair<K4,D4>
        >& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }
    
	// n = 5
    
    template<class Archive,
    typename K1,typename K2,typename K3,typename K4,typename K5,
    typename D1,typename D2,typename D3,typename D4,typename D5
    >
    void serialize(
        Archive & ar,
        boost::fusion::map<
            boost::fusion::pair<K1,D1>, 
            boost::fusion::pair<K2,D2>, 
            boost::fusion::pair<K3,D3>,
            boost::fusion::pair<K4,D4>,
            boost::fusion::pair<K5,D5>
        >& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }

}}

#endif
