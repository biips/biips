///////////////////////////////////////////////////////////////////////////////
// keyword.hpp                                                               //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_KEYWORD_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_KEYWORD_HPP_ER_2010
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/vector/vector0.hpp>

#include <boost/parameter/keyword.hpp>
#include <boost/parameter/parameters.hpp>

#include <boost/parameter/detail/unpack_parameters.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace factor{

    template<typename Key>
    struct kwd{
        typedef kwd<Key> tag_;
        static ::boost::parameter::keyword<tag_> & instance;
    };

    template<typename Key>
		::boost::parameter::keyword< kwd<Key> > &
    kwd<Key>::instance = ::boost::parameter::keyword< kwd<Key> >::get();

    template<typename Keys>
    struct par_spec{
        
        typedef typename boost::mpl::fold<
            Keys,
            boost::mpl::vector0<>,
            boost::mpl::push_back<
                boost::mpl::_1,
                boost::parameter::required<
                    factor::kwd<boost::mpl::_2>
                >
            >
        >::type vec_required;
        
        typedef typename boost::parameter::detail::unpack_parameters<
           vec_required
        >::type type;
        
    };

}// factor
}// contingency_table
}// detail
}// statistics
}// boost

#endif
