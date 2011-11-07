//////////////////////////////////////////////////////////////////////////////
// statistics::detail::fusion::at_key::iterator.hpp                         //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_FUSION_AT_KEY_ITERATOR_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_FUSION_AT_KEY_ITERATOR_HPP_ER_2009
#include <boost/utility/result_of.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/statistics/detail/fusion/at_key/functor.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace fusion{
namespace at_key{

    template<typename It,typename K>
    struct meta_iterator
    {
        typedef statistics::detail::fusion::at_key::functor<K> f_;
        typedef typename iterator_reference<It>::type ref1_;
        
        // See boost.user mailing list
        // Subject: [transform_iterator]'s reference
        // 10/05/2009
                
        typedef typename result_of<f_(ref1_)>::type ref2_;
        typedef boost::transform_iterator<f_,It,ref2_> type;
    
        static type call(It i){
            // don't use make_transform_iterator because not default.
            return type(i,f_());
        }
    
    };

    template<typename K,typename It>
    typename meta_iterator<It,K>::type
    make_iterator(It i){
        typedef meta_iterator<It,K> m_;
        return m_::call(i);
    }

}// at_key
}// fusion
}// detail
}// statistics
}// boost

#endif