///////////////////////////////////////////////////////////////////////////////
// sequence_to_map.hpp                                                       //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_FUSION_CONTAINER_MAP_DETAIL_SEQUENCE_TO_MAP_HPP_ER_2010
#define BOOST_FUSION_CONTAINER_MAP_DETAIL_SEQUENCE_TO_MAP_HPP_ER_2010
#include <boost/fusion/container/map/detail/sequence_to_pairs.hpp>
#include <boost/fusion/container/map/detail/unpack_map.hpp>

namespace boost{
namespace fusion{
namespace detail{

    // sequence_to_map<mpl::vector<K1,D1,...,Kn,Dn> >
    template<typename T>
    struct sequence_to_map{
    
        typedef typename 
            boost::fusion::detail::sequence_to_pairs<T> traits_;        
        typedef typename traits_::keys_ keys_;
        typedef typename traits_::datas_ datas_;
        typedef typename traits_::type pairs_;
        typedef typename boost::fusion::detail::unpack_map<pairs_>::type type;

    };

}// detail
}// fusion
}// boost
    
#endif
