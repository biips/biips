//////////////////////////////////////////////////////////////////////////////
// detail::multi_array::serialization::serialize.hpp                        //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_MULTI_ARRAY_SERIALIZATION_SERIALIZE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_MULTI_ARRAY_SERIALIZATION_SERIALIZE_HPP_ER_2009
#include <boost/statistics/detail/multi_array/serialization/save.hpp>
#include <boost/statistics/detail/multi_array/serialization/load.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost{ namespace serialization{

    template<class Archive,typename T>
    inline void serialize(
        Archive & ar,
        boost::multi_array<T,2>& t,
        const unsigned int file_version
    )
    {
        split_free(ar, t, file_version);
    }

}}

#endif
