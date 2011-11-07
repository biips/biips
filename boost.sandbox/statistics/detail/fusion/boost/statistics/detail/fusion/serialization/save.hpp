//////////////////////////////////////////////////////////////////////////////
// detail::fusion::serialization::save.hpp                             		//
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_FUSION_SERIALIZATION_SAVE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_FUSION_SERIALIZATION_SAVE_HPP_ER_2009
#include <boost/utility/enable_if.hpp>
#include <boost/fusion/support/is_sequence.hpp>
#include <boost/statistics/detail/fusion/serialization/saver.hpp>

namespace boost{ namespace serialization{


    template<typename Archive,typename S>
    inline typename boost::enable_if<
    	boost::fusion::traits::is_sequence<S>,
        void
    >::type
    save(
        Archive & ar,
        const S & t,
        const unsigned int file_version
    )
	{
		boost::statistics::detail::fusion::serialization::make_saver(ar)(t);
    }

}// serialization
}// boost

#endif