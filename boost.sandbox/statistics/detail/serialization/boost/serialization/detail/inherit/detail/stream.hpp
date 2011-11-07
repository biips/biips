//////////////////////////////////////////////////////////////////////////////
// serialization::detail::inherit::detail::stream.hpp         				//
//                                                                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_SERIALIZATION_DETAIL_INHERIT_DETAIL_STREAM_HPP_ER_2010
#define BOOST_SERIALIZATION_DETAIL_INHERIT_DETAIL_STREAM_HPP_ER_2010
#include <ostream>
#include <boost/mpl/int.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/count_if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/empty_base.hpp>

namespace boost{
namespace serialization{
namespace detail{
namespace impl_inherit2_stream{
	template<int K> struct os_caller;
	template<typename T,int K> struct count;
}

	template<typename T1,typename T2>
    struct inherit2;

	// Writes a comma separated list of the instances of the non-empty
	// "terminal types" within the nested structure inherit2<T1,T2>. A terminal 
    // type is one that is not an instance of inherit2.
    // For example, if these types are A,B,C, with descriptions 'a, 'b' and 'c',
    // "a,b,c" is written to os.
    template<typename A,typename B,typename T1,typename T2>
	std::basic_ostream<A,B>&
    operator<<(std::basic_ostream<A,B>& os,const inherit2<T1,T2>& that){
		typedef inherit2<T1,T2> in_;
        static const int k = impl_inherit2_stream::count<in_,0>::result;
    	typedef impl_inherit2_stream::os_caller<k> caller_;
        return caller_::top_call(os,that);
    }

namespace impl_inherit2_stream{

	typedef boost::mpl::empty_base ignore_;

	// Terminal node
	template<typename T,int K>
    struct count{
		BOOST_STATIC_CONSTANT(int,result = K+1);
    };

	template<int K>
    struct count<ignore_,K>{
		BOOST_STATIC_CONSTANT(int,result = K);
    };

	// Non-terminal node
	template<typename T1,typename T2,int K>
    struct count<inherit2<T1,T2>,K>{
    	typedef count<T2,K+1> next_;
		BOOST_STATIC_CONSTANT(int,result = next_::result);
    };

	template<typename T2,int K>
    struct count<inherit2<ignore_,T2>,K>{
    	typedef count<T2,K> next_;
		BOOST_STATIC_CONSTANT(int,result = next_::result);
    };

	// os caller ensures that the output for say 3 items, a, b, and c is
    // "a,b,c", rather than "a,b,c,"
	template<int K>
	struct os_caller{
        struct silent{
			template<typename T>
            static const T& call(const T& t){
            	return t;
            }
        };
		
        static int count;
        
		template<typename S,typename I>
    	static S& top_call(S& os,const I& t){
        	count = 0;
    		return call(os,t);
        }

		template<typename S>
    	static S& call(S& os,const ignore_& t){
    		return os;
        }

		template<typename S,typename T>
    	static S& call(S& os,const T& t){
        	if(K == count+1){
				os << t;            
            }else{
            	os << t << ',';            
            	++count;
            }
            return os;
        }

		template<typename S,typename T1,typename T2>
    	static S& call(S& os,const inherit2<T1,T2>& t){
    		call(os,silent::template call<T1>(t));
            return call(os,silent::template call<T2>(t));
        }

		template<typename S,typename T2>
    	static S& call(S& os,const inherit2<ignore_,T2>& t){
    		return call(os,silent::template call<T2>(t));
        }

		template<typename S,typename T1>
    	static S& call(S& os,const inherit2<T1,ignore_>& t){
    		return call(os,silent::template call<T1>(t));
        }

		template<typename S>
    	static S& call(S& os,const inherit2<ignore_,ignore_>& t){
    		return os;
        }

    };
	template<int K>
    int os_caller<K>::count = 0;

}// impl_inherit2_stream

}// detail
}// serialization
}// boost

#endif
