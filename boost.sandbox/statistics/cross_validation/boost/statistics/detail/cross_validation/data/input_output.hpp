//////////////////////////////////////////////////////////////////////////////
// cross_validation::data::input_output.hpp                                 //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_DATA_INPUT_OUTPUT_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_DATA_INPUT_OUTPUT_HPP_ER_2009
#include <boost/mpl/int.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/include/at_key.hpp>

#include <boost/fusion/support/pair.hpp>
#include <boost/fusion/include/pair.hpp>

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/map_fwd.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace data{

    // Data-type for (x,f(x))
    //
    // Note: the library does not require a specific data-type; this one is
    // provided as a convenience
    template<
        typename X,
        typename F,
        typename Kx = mpl::int_<1>,
        typename Ky = mpl::int_<2>
    >
    struct meta_input_output{
        
        struct input{
            typedef Kx key_type;
            typedef X data_type;
            typedef boost::fusion::pair<key_type,data_type> type;
        };
        struct output{
            typedef Ky key_type;
            typedef typename boost::result_of<F(const X&)>::type data_type;
            typedef boost::fusion::pair<key_type,data_type>    type;
        };
    
        typedef boost::fusion::map<
            typename input::type,
            typename output::type
        > type;

        template<typename X1>
        static type make(const X1& x,const F& f){
            typedef typename input::key_type k_x_;
            typedef typename output::key_type k_y_;
            
            return type(
                boost::fusion::make_pair<k_x_>(x),
                boost::fusion::make_pair<k_y_>(
                    f(x)
                )
            );
        }
        
    };
    
}// data
}// cross_validation
}// detail
}// statistics
}// boost

#endif