//////////////////////////////////////////////////////////////////////////////
// statistics::detail::fusion::at_key::functor.hpp                          //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_FUSION_AT_KEY_FUNCTOR_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_FUSION_AT_KEY_FUNCTOR_HPP_ER_2009
#include <boost/utility/result_of.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/mpl/identity.hpp>

#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/include/at_key.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace fusion{
namespace at_key{

    template<typename K>
    class functor{
    
        // M has to be a fusion::map
        template<typename M> 
        struct result_impl
            : boost::fusion::result_of::at_key<M, K>{};

        public:

        template<typename S> 
        struct result{};
    
        template<typename F,typename M>
        struct result<F(M)>
            : result_impl<typename remove_reference<M>::type>{};

        template<typename S>
        struct sig : result<S>{};
    
        template<typename M>
        typename result_impl<const M>::type 
        operator()(const M& p)const{
            return boost::fusion::at_key<K>(p);
        }
    
        template<typename M>
        typename result_impl<M>::type 
        operator()(M& p)const{
            return boost::fusion::at_key<K>(p);
        }
    
    };
    
}// at_key
}// fusion
}// detail
}// statistics
}// boost

#endif
