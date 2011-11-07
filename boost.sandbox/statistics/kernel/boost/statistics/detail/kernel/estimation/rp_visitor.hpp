///////////////////////////////////////////////////////////////////////////////
// kernel::functional::rp_visitor.hpp                                        //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
/////////////////////////////////////////////////////////////////////////////// 
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_RP_VISITOR_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_RP_VISITOR_HPP_ER_2009
#include <boost/type_traits/is_reference.hpp>
#include <boost/mpl/not.hpp>
#include <boost/call_traits.hpp>
#include <boost/statistics/detail/kernel/estimation/detail/mean_accumulator.hpp>
//#include <boost/statistics/detail/kernel/estimation/detail/return_if.hpp>
//#include <boost/statistics/detail/kernel/estimation/detail/range_difference.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{

// This visitor, f, keeps a data point, x0, and each time f(x) is called, it 
// updates an estimate of the density at x0, p(x0), by the Rosenblatt-Parzen 
// method, using a given kernel k.
//
// The estimate is the average of the kernel evaluations over the traversed 
// dataset. That average is implemented by an accumulator of type A.
//
// X can be a reference which is only recommended if the x object is expensive 
// to copy
template<
    typename K,
    typename X,
    typename A = typename 
        statistics::detail::kernel::detail::mean_accumulator<typename K::result_type>::type 
>
class rp_visitor : K{ //, addable<rp_visitor<K,X,A> >{ // 
    typedef is_reference<X> is_ref_;
    public:
    typedef K kernel_type;
    typedef A accumulator_type;
    typedef typename K::result_type result_type;

    // Construct
    rp_visitor();
    rp_visitor(typename call_traits<X>::param_type);
    rp_visitor(
        K k, // passing radius calls implicit conversion
        typename call_traits<X>::param_type x
    );
    rp_visitor(
        K k,
        typename call_traits<X>::param_type,
        const accumulator_type&
    );
    rp_visitor(const rp_visitor&);
    rp_visitor& operator=(const rp_visitor&);
    
    // Update
    // Passing the training data x1 updates the estimator
    template<typename X1> result_type operator()(const X1& x1);
    
    // Access
    typename call_traits<X>::const_reference x()const;
    const A& accumulator()const;
    const result_type& normalizing_constant()const;

    result_type estimate()const; 

    private:
    typename call_traits<X>::value_type x_;
    A acc_;
};

//Construction
template<typename K,typename X,typename A>
rp_visitor<K,X,A>::rp_visitor(){
    BOOST_MPL_ASSERT((
        mpl::not_<is_ref_>
    ));
}

template<typename K,typename X,typename A>
rp_visitor<K,X,A>::rp_visitor(K k,typename call_traits<X>::param_type x)
:K(k),x_(x),acc_(){}
    
template<typename K,typename X,typename A>
rp_visitor<K,X,A>::rp_visitor(
    K k,
    typename call_traits<X>::param_type x,
    const A& a
):K(k),x_(x),acc_(a){}

template<typename K,typename X,typename A>
rp_visitor<K,X,A>::rp_visitor(const rp_visitor& that)
:K(static_cast<const K&>(that)),x_(that.x_),acc_(that.acc_){}

template<typename K,typename X,typename A>
typename rp_visitor<K,X,A>::rp_visitor& 
rp_visitor<K,X,A>::operator=(const rp_visitor& that){
    if(&that!=this){
        BOOST_MPL_ASSERT((mpl::not_<is_ref_>));
        K::operator=(static_cast<const K&>(*that)); 
        x_ = that.x_;
        acc_ = that.acc_;
    }   
    return *this;
}

// Evaluate
template<typename K,typename X,typename A>
template<typename X1>
typename rp_visitor<K,X,A>::result_type
rp_visitor<K,X,A>::operator()(const X1& x1){
    const K& kernel = static_cast<const K&>(*this);
    result_type t = kernel(x(),x1);
    this->acc_(t);
    return t;
}


// Access
template<typename K,typename X,typename A>
typename rp_visitor<K,X,A>::result_type
rp_visitor<K,X,A>::estimate()const{
    return accumulators::mean(
        this->accumulator()
    );
}

template<typename K,typename X,typename A>
const A&
rp_visitor<K,X,A>::accumulator()const{
    return  this->acc_;
}
    
template<typename K,typename X,typename A>
const typename rp_visitor<K,X,A>::result_type&
rp_visitor<K,X,A>::normalizing_constant()const{
    const K& k = static_cast<const K&>(*this);
    return k.normalizing_constant();
}

template<typename K,typename X,typename A>
typename call_traits<X>::const_reference
rp_visitor<K,X,A>::x()const{
    return this->x_;
}
    

}// kernel
}// detail
}// statistics
}// boost

#endif