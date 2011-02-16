///////////////////////////////////////////////////////////////////////////////
// subset_traits.hpp                                                         //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_FUSION_DETAIL_CONTAINER_MAP_SUBSET_TRAITS_HPP_ER_2010
#define BOOST_FUSION_DETAIL_CONTAINER_MAP_SUBSET_TRAITS_HPP_ER_2010
#include <boost/mpl/bool.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/fusion/include/filter_view.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/container/map/detail/unpack_map.hpp>

namespace boost { 
namespace fusion{
namespace detail{

    template<typename T,typename Keys>
    struct map_subset_traits{
    
        template<typename U>
        struct key_of{ typedef typename U::first_type type; };
    
        template<typename Key>
        struct contains : boost::mpl::contains<Keys,Key>{};

        // TODO check that it contains Keys
        //typedef typename boost::fusion::detail::extract_keys<
        //    T
        //>::type keys_superset;
        
        struct filter{
            template<typename U>
            struct apply : contains<typename key_of<U>::type>{};
        };
                
        typedef boost::fusion::filter_view<T,filter> view;
        typedef boost::fusion::filter_view<
            typename boost::add_const<T>::type,
            filter
        > view_const;

        template<typename Key>
        struct pair_of{ 
            typedef typename boost::fusion::result_of::at_key<T,Key>::type at_;
            typedef typename boost::remove_reference<at_>::type data_;
            typedef boost::fusion::pair<Key,data_> type; 
        };

        typedef typename boost::mpl::fold<
            Keys,
            boost::mpl::vector0<>,
            boost::mpl::push_back<
                boost::mpl::_1,
                pair_of<boost::mpl::_2>
            >
        >::type pairs;
        
        typedef typename boost::fusion::detail::unpack_map<
            pairs
        >::type map;
    };


}// detail
}// fusion
}// boost

#endif
