///////////////////////////////////////////////////////////////////////////////
// partition_by_position_parity.hpp                                          //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_MPL_DETAIL_PARTITION_BY_POSITION_PARITY_HPP_ER_2010
#define BOOST_MPL_DETAIL_PARTITION_BY_POSITION_PARITY_HPP_ER_2010

#include <boost/mpl/at.hpp>
#include <boost/mpl/bind.hpp>
#include <boost/mpl/inserter.hpp>
#include <boost/mpl/math/is_even.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/zip_view.hpp>
#include <boost/mpl/partition.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/unpack_args.hpp>

namespace boost { 
namespace mpl{
namespace detail{
    
    // Usage:
    //     partition_by_position_parity< vector<a,x,b,y> >::type  
    // equivalent to
    //     pair< vector<a,b>, vector<x,y> >
    template<typename T>
    struct partition_by_position_parity{
    
        typedef typename boost::mpl::size<T>::type size_;
        typedef boost::mpl::range_c<int,0,size_::value> range_;
        typedef boost::mpl::zip_view< boost::mpl::vector<T, range_> > zip_view_;
        
        template<typename Seq>
        struct fetch : boost::mpl::apply<
            boost::mpl::unpack_args<boost::mpl::_1>,
            Seq
        >{};
        
        struct inserter : boost::mpl::inserter<
           boost::mpl::vector0<>,
           boost::mpl::push_back<
                boost::mpl::_1,
                fetch<boost::mpl::_2>
            > 
        >{};

        typedef typename boost::mpl::partition<
            zip_view_
            , boost::mpl::unpack_args< boost::mpl::is_even<boost::mpl::_2> >,
            inserter,
            inserter
        >::type  type;    
        
    };
    
}// detail
}// mpl
}// boost

#endif
