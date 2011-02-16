///////////////////////////////////////////////////////////////////////////////
// tree_view::dynamic_stage.hpp                                              //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_TREE_VIEW_DYNAMIC_STAGE_HPP_ER_2009
#define BOOST_TREE_VIEW_DYNAMIC_STAGE_HPP_ER_2009
#include <stdexcept>
#include <ostream>
#include <boost/mpl/range_c.hpp>
#include <boost/tree_view/stage.hpp>
#include <boost/switch.hpp>

namespace boost{
namespace tree_view{

    // A runtime (a.k.a dynamic) version of stage
    //
    // n : number of branches
    // m : number of stages
    template<unsigned n,unsigned m = BOOST_SWITCH_LIMIT>
    class dynamic_stage{
        typedef unsigned size_;
        typedef mpl::range_c<size_,0,m> mpl_range_;

        public:
        static unsigned position_first(unsigned i);
        static unsigned position_last(unsigned i);
        static unsigned number_nodes(unsigned i);
        
        private:
        template<template<typename> class F>
        struct switch_case{
            typedef size_ result_type;
            switch_case(){}
            template<class Case>
            result_type operator()(Case)const{
                typedef stage<Case::value,n> stage_;
                return F<stage_>::get();
            }
        };
        struct throw_out_of_range {
            template<class Case>
            void operator()(Case) const {
                throw(std::out_of_range());
            }
        };
        template<template<typename> class F>
        static unsigned switch_(unsigned j);
    };

    // Implementation //

    template<unsigned n,unsigned m>
    template<template<typename> class F>
    unsigned dynamic_stage<n,m>::switch_(unsigned j){
        return boost::switch_<mpl_range_>(
            j, 
            switch_case<F>()//, 
            //throw_out_of_range()
        );
    }

    template<unsigned n,unsigned m>
    unsigned dynamic_stage<n,m>::position_first(unsigned j){
        return switch_<position_first_>(j);
    }
    template<unsigned n,unsigned m>
    unsigned dynamic_stage<n,m>::position_last(unsigned j){
        return switch_<position_last_>(j);
    }

    template<unsigned n,unsigned m>
    unsigned dynamic_stage<n,m>::number_nodes(unsigned j){
        return switch_<number_nodes_>(j);
    }

}// tree_view
}// boost

#endif
