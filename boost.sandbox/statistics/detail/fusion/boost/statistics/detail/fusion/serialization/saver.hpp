//////////////////////////////////////////////////////////////////////////////
// detail::fusion::serialization::saver.hpp                            		//
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_FUSION_SERIALIZATION_SAVER_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_FUSION_SERIALIZATION_SAVER_HPP_ER_2009
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/support/is_sequence.hpp>
#include <boost/fusion/support/tag_of.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace fusion{
namespace serialization{

// Implementation follows from
// http://www.boost.org/doc/libs/1_40_0/libs/fusion/doc/html/fusion/quick_start.html
// under AssociativeContainer
template<typename Oa>
struct saver
{

	saver(Oa& ar):ar_(ar){}
    saver(const saver& that):ar_(that.ar_){}

	// TODO overload with any other non sequence fusion data structure

    // template <typename F,typename S>
    // void operator()(const boost::fusion::pair<F,S>& data)const
    // {
    //     (*this)(data.second);
    // }

    template <typename S>
    typename boost::enable_if<
    	boost::fusion::traits::is_sequence<S>,
        void
    >::type
    operator()(const S& seq)const
    {
		boost::fusion::for_each(seq,(*this));
    }

    template <typename T>
    typename boost::enable_if<
    	boost::is_same<
        	typename boost::fusion::detail::tag_of<T>::type,
            boost::fusion::non_fusion_tag
        >,
        void
    >::type
    operator()(const T& serializable_item)const
    {
        // Note : tag_of< fusion::pair<F,S> > == non_fusion_tag 
    	// and operator<<(OStream& is, pair<First, Second>& p) is defined in 
        // fusion       

		this->ar_ << serializable_item;
    }

	private:
	mutable Oa& ar_;    
};

template<typename Oa>
saver<Oa> 
make_saver(Oa& ar){ return saver<Oa>(ar); }

}// serialization
}// fusion
}// detail
}// statistics
}// boost

#endif





