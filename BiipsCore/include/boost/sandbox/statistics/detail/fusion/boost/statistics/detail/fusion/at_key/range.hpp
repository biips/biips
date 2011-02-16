//////////////////////////////////////////////////////////////////////////////
// statistics::detail::fusion::at_key::range.hpp                            //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_FUSION_AT_KEY_RANGE_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_FUSION_AT_KEY_RANGE_HPP_ER_2009
#include <boost/range.hpp>
#include <boost/statistics/detail/fusion/at_key/iterator.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace fusion{
namespace at_key{

    template<typename It,typename K>
    struct meta_range
    {
        typedef meta_iterator<It,K> meta_;
        
        typedef typename meta_::type it_;
        typedef iterator_range<it_>  type;
    
        static type call(It b,It e){
            return type(
                make_iterator<K>(b),
                make_iterator<K>(e)
            );
        }
    
    };

    template<typename K,typename It>
    typename meta_range<It,K>::type
    make_range(It b,It e){
        typedef meta_range<It,K> m_;
        return m_::call(b,e);
    }

}// at_key
}// fusion
}// detail
}// statistics
}// boost

#endif
