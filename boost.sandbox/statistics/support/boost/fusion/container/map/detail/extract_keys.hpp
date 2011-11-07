///////////////////////////////////////////////////////////////////////////////
// extract_keys.hpp                                                          //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_FUSION_CONTAINER_MAP_DETAIL_EXTRACT_KEYS_HPP_ER_2010
#define BOOST_FUSION_CONTAINER_MAP_DETAIL_EXTRACT_KEYS_HPP_ER_2010
#include <boost/mpl/fold.hpp>
#include <boost/mpl/vector/vector0.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/fusion/mpl.hpp> // likely to be needed by client

namespace boost { 
namespace fusion{
namespace detail{
    
    template<typename Map>
    struct extract_keys{
    
        template<typename P>
        struct extract_first{
           typedef typename P::first_type type;
        };
    
        typedef typename Map::storage_type storage_;
        typedef typename boost::mpl::fold<
            storage_,
            boost::mpl::vector0<>,
            boost::mpl::push_back<
                boost::mpl::_1,
                extract_first<boost::mpl::_2> 
            >
        >::type type;
    };
    
}// detail
}// fusion
}// boost

#endif
