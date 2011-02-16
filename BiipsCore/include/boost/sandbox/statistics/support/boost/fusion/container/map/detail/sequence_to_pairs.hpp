///////////////////////////////////////////////////////////////////////////////
// sequence_to_pairs.hpp                                                     //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_FUSION_CONTAINER_MAP_DETAIL_SEQUENCE_TO_PAIRS_HPP_ER_2010
#define BOOST_FUSION_CONTAINER_MAP_DETAIL_SEQUENCE_TO_PAIRS_HPP_ER_2010
#include <boost/fusion/include/pair.hpp>

#include <boost/mpl/at.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/bind.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/unpack_args.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <boost/mpl/zip_view.hpp>
#include <boost/mpl/transform_view.hpp>

#include <boost/mpl/detail/partition_by_position_parity.hpp>

namespace boost{
namespace fusion{
namespace detail{

    // sequence_to_pairs<mpl::vector<K1,D1,...,Kn,Dn> >::type
    template<typename T>
    struct sequence_to_pairs{
    
        typedef typename boost::mpl::detail::partition_by_position_parity<
             T>::type partition_;
        typedef typename partition_::first keys_;
        typedef typename partition_::second datas_;
        
        typedef boost::mpl::transform_view<
           boost::mpl::zip_view< boost::mpl::vector2<keys_,datas_> >
           , boost::mpl::unpack_args< 
               boost::fusion::pair<boost::mpl::_1,boost::mpl::_2> 
            >
        > type;
        
    };

}// detail
}// fusion
}// boost
    
#endif
