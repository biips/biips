//////////////////////////////////////////////////////////////////////////////
// serialization::detail::inherit::inherit_linearly.hpp                     //
//                                                                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_SERIALIZATION_DETAIL_INHERIT_INHERIT_LINEARLY_HPP_ER_2010
#define BOOST_SERIALIZATION_DETAIL_INHERIT_INHERIT_LINEARLY_HPP_ER_2010
#include <boost/mpl/empty_base.hpp>
#include <boost/mpl/reverse_fold.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/aux_/na_fwd.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/serialization/base_object.hpp>

#include <boost/concept/assert.hpp>
#include <boost/serialization/detail/inherit/detail/inherit2.hpp>

namespace boost{
namespace serialization{
namespace detail{

    // This serializable class is the result of merging an mpl sequence, S, of 
    // serializable class types. It has
    // - A contructor that takes an argument pack, and
    // - Writes a comma separated list of its components to an output stream,
	// provided each of its components has the corresponding interface.
    //
    // Let arg denote an argument pack and T one element of S
    // Construction :	inherit_linearly<S> obj(args);
    // Access:			static_cast<const T&>(obj);
    // Saving:			oa << obj;
    // Loading:			ia >> obj;
    // Description:		os << obj
    //
    // See Boost.Parameter regarding argument pack.
    // See sandbox/statistics/data for a small application
	//
	// Background : See class by the same name in the MPL framework.

	template<typename S,typename R = boost::mpl::empty_base>
    struct inherit_linearly : boost::mpl::reverse_fold<
    	S,R,inherit2<boost::mpl::_2,boost::mpl::_1>
    >::type{

		typedef S components;
        
        typedef typename boost::mpl::reverse_fold<
        	S,R,inherit2<boost::mpl::_2,boost::mpl::_1>
    	>::type super_;

        inherit_linearly(){}
            
        template<typename Args>
        inherit_linearly(const Args& args):super_(args){}

		private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            // serialize base class information
            ar & boost::serialization::base_object<super_>(*this);
        }                  

    };

	// If a, b, c are instances of the subset of non-empty bases in S,
    // writes "(a,b,c)" to os
    template<typename A,typename B,typename S,typename R>
	std::basic_ostream<A,B>&
    operator<<(std::basic_ostream<A,B>& os,const inherit_linearly<S,R>& that){
		typedef typename inherit_linearly<S,R>::super_ super_;
        return os << '(' << static_cast<const super_&>(that) << ')';
    }

}// detail
}// serialization
}// boost

#endif



