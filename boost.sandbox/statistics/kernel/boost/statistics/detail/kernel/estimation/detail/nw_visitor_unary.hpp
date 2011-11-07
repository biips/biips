///////////////////////////////////////////////////////////////////////////////
// kernel::functional::nw_visitor_unary.hpp                                  //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
/////////////////////////////////////////////////////////////////////////////// 
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_DETAIL_NW_VISITOR_UNARY_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_DETAIL_NW_VISITOR_UNARY_HPP_ER_2009
#include <boost/type_traits.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/call_traits.hpp>
#include <boost/statistics/detail/kernel/estimation/detail/mean_accumulator.hpp>
#include <boost/statistics/detail/kernel/estimation/nw_visitor.hpp>
#include <boost/concept_check.hpp>
#include <boost/concept/assert.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{
namespace detail{

// This visitor, g, when called as g(t), forwards to f(f1(t),f2(t)), where f
// is an instance of nw_visitor, and f1(t) and f2(t) are the x and y components.
//
// K, X, A: See rp_visitor
template<
    typename K,
    typename X,
    typename F1,    // x extractor
    typename F2,    // y extractor
    typename A = typename 
        statistics::detail::kernel::detail::mean_accumulator<
            typename K::result_type
        >::type 
>
class nw_visitor_unary : public nw_visitor<K,X,A>{
    typedef nw_visitor<K,X,A> super_;
    
    public:
    typedef F1 input_extractor_type;
    typedef F2 output_extractor_type;
    
    typedef super_ nw_visitor_type;
    typedef typename super_::result_type result_type;

    // Construction
    nw_visitor_unary(){}
    nw_visitor_unary(
        K k,
        typename call_traits<X>::param_type x
    ):super_(k,x){
        BOOST_CONCEPT_ASSERT((
            boost::DefaultConstructible<F1>
        ));
        BOOST_CONCEPT_ASSERT((
            boost::DefaultConstructible<F2>
        ));
    }
    nw_visitor_unary(
        K k,
        typename call_traits<X>::param_type x,
        const A& a
    ):super_(k,x,a)
    {
        BOOST_CONCEPT_ASSERT((
            boost::DefaultConstructible<F1>
        ));
        BOOST_CONCEPT_ASSERT((
            boost::DefaultConstructible<F2>
        ));
    }
        
    nw_visitor_unary(const nw_visitor_unary& that)
    :super_(that),f1_(that.f1_),f2_(that.f2_){}
    
    nw_visitor_unary& operator=(const nw_visitor_unary& that)
    {
        if(&that!=this){
            super_::operator=(that);
            this->f1_ = that.f1_;
            this->f2_ = that.f2_;
        }
        return *this;
    }

        
    // Update
    template<typename T>
    result_type operator()(const T& t)
    {
        const F1& crf1 = this->f1_;
        const F2& crf2 = this->f2_;
        return static_cast<super_&>(*this)( 
            crf1(t),
            crf2(t)
        );
    }

    private:
    F1 f1_;
    F2 f2_;

};

}// detail
}// kernel
}// detail
}// statistics
}// boost

#endif
