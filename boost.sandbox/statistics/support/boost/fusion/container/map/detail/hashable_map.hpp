///////////////////////////////////////////////////////////////////////////////
// hashable_map.hpp                                                          //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_FUSION_CONTAINER_MAP_DETAIL_HASHABLE_MAP_HPP_ER_2010
#define BOOST_FUSION_CONTAINER_MAP_DETAIL_HASHABLE_MAP_HPP_ER_2010
#include <boost/mpl/assert.hpp>
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/comparison.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/container/map/detail/subset_traits.hpp>

namespace boost { 
namespace fusion{
namespace detail{

    template<typename Map>
    struct hashable_map : Map
    {
        typedef typename fusion::detail::extract_keys<Map>::type keys_;
        
        template<typename T>
        hashable_map(const T& m) 
        : Map(
            typename fusion::detail::map_subset_traits<T,keys_>::view_const(m)
        ){
           	typedef fusion::detail::map_subset_traits<T,keys_> traits_;
           	// BOOST_MPL_ASSERT_NOT(( traits_::superset_contains_keys )); 
        }
        
    };

namespace hashable_map_aux{

    struct hash_combine{
    
        typedef void result_type;
        typedef std::size_t size_;

        hash_combine(size_& s):seed(s){}
        
        template<typename T>
        result_type operator()(const T& elem)const{
            boost::hash_combine(seed, elem.second);
        }
        
        private:
        mutable size_& seed;
    };

}// hashable_map_aux

    template<typename Map>
    std::size_t hash_value(const fusion::detail::hashable_map<Map>& m) {
        namespace ns = fusion::detail::hashable_map_aux;
        std::size_t seed = 0;
        typedef ns::hash_combine hash_;
        boost::fusion::for_each(m, hash_(seed) );
        return seed;
    }

}// detail
}// fusion
}// boost

#endif
