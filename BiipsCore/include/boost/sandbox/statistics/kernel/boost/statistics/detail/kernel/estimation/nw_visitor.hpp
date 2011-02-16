///////////////////////////////////////////////////////////////////////////////
// kernel::functional::nw_visitor.hpp                                        //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
/////////////////////////////////////////////////////////////////////////////// 
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_NW_VISITOR_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_NW_VISITOR_HPP_ER_2009
#include <boost/type_traits/is_reference.hpp>
#include <boost/mpl/not.hpp>
#include <boost/call_traits.hpp>
#include <boost/statistics/detail/kernel/estimation/detail/mean_accumulator.hpp>
#include <boost/statistics/detail/kernel/estimation/rp_visitor.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{
   
// This visitor, f, updates a Nadaraya-Watson estimate of E[Y|X=x0] each
// time f(x,y) is called. 
//
// K,X,A : See rp_visitor 
template<
    typename K,
    typename X,
    typename A = typename 
        statistics::detail::kernel::detail::mean_accumulator<typename K::result_type>::type 
>
class nw_visitor{
    public:
    typedef rp_visitor<K,X,A> rp_visitor_type;
    typedef typename rp_visitor_type::result_type result_type;
    typedef K kernel_type;
    typedef A accumulator_type;
        
    //Construct
    nw_visitor();
    nw_visitor(typename call_traits<X>::param_type);
    nw_visitor(
        K k, // passing radius should call implicit conversion
        typename call_traits<X>::param_type x  
    );
    nw_visitor(
        K k,
        typename call_traits<X>::param_type,
        const accumulator_type&
    );
    nw_visitor(const nw_visitor&);
    nw_visitor& operator=(const nw_visitor&);
        
    // Update
    protected:
    template<typename X1,typename Y1> // Training data point
    result_type operator()(const X1& x1,const Y1& y1);

    public:
    // Access
    result_type unnormalized_estimate()const;
    result_type normalizing_constant()const;
    result_type estimate()const; 

    
    const A& accumulator()const;
    const rp_visitor_type& rp_visitor()const;
        
    private:
    rp_visitor_type rp_visitor_;
    A a_;
};
    
//Construction
template<typename K,typename X,typename A>
nw_visitor<K,X,A>::nw_visitor():rp_visitor_(),a_(){}
    
template<typename K,typename X,typename A>
nw_visitor<K,X,A>::nw_visitor(K k,typename call_traits<X>::param_type x)
:rp_visitor_(k,x),a_(){}
    
template<typename K,typename X,typename A>
nw_visitor<K,X,A>::nw_visitor(
    K k,typename call_traits<X>::param_type x,const A& a
):rp_visitor_(k,x,a),a_(a){}
    
template<typename K,typename X,typename A>
nw_visitor<K,X,A>::nw_visitor(const nw_visitor& that)
:rp_visitor_(that.rp_visitor_),a_(that.a_){}
    
template<typename K,typename X,typename A>
typename nw_visitor<K,X,A>::nw_visitor& 
nw_visitor<K,X,A>::operator=(const nw_visitor& that){
    if(&that!=this){
        rp_visitor_ = that.rp_visitor_;
        a_ = that.a_;
    }   
    return *this;
}
    
// Update
template<typename K,typename X,typename A>
template<typename X1,typename Y1>
typename nw_visitor<K,X,A>::result_type
nw_visitor<K,X,A>::operator()(const X1& x1,const Y1& y){
    result_type w = (this->rp_visitor_(x1));
    result_type wy = w * y; 
    this->a_(wy);
    return wy;
}

// Access
template<typename K,typename X,typename A>
typename nw_visitor<K,X,A>::result_type
nw_visitor<K,X,A>::unnormalized_estimate()const{
    return accumulators::mean(
        this->accumulator()
    );
}

template<typename K,typename X,typename A>
typename nw_visitor<K,X,A>::result_type
nw_visitor<K,X,A>::normalizing_constant()const{
    return (this->rp_visitor_).estimate();
}
    
template<typename K,typename X,typename A>
typename nw_visitor<K,X,A>::result_type
nw_visitor<K,X,A>::estimate()const{
    return (this->unnormalized_estimate()/this->normalizing_constant());
}
    
template<typename K,typename X,typename A>
const A& nw_visitor<K,X,A>::accumulator()const{ return  this->a_; }
        
template<typename K,typename X,typename A>
const rp_visitor<K,X,A>& 
nw_visitor<K,X,A>::rp_visitor()const{
    return (this->rp_visitor_);
}
    
}// kernel
}// detail
}// statistics
}// boost

#endif