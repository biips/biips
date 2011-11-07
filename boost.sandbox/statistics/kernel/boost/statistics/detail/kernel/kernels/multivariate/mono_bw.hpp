///////////////////////////////////////////////////////////////////////////////
// kernel::multivariate::mono_bw.hpp                                         //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICEMSE_1_0.txt or copy at http://www.boost.org/LICEMSE_1_0.txt)         //
/////////////////////////////////////////////////////////////////////////////// 
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_KERNELS_MULTIVARIATE_MONO_BW_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_KERNEL_KERNELS_MULTIVARIATE_MONO_BW_HPP_ER_2009
#include <cmath>
#include <numeric>
#include <boost/lambda/lambda.hpp>
#include <boost/vector_space/data/lazy_difference.hpp>
#include <boost/statistics/detail/kernel/kernels/multivariate/crtp.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{
namespace multivariate{

// Overview: Multivariate kernel, common bandwidth across coordinates.
//
// Notation: Let x denote a vector of size M,
//
// Usage:
// typedef mono_bw_kernel<scalar::gaussian_kernel<T>,M> mult_;
// typedef mult_::result_type result_;
// mult_ mult(bandwidth);
// result_ r = mult(x);
template<typename K,unsigned M>
class mono_bw_kernel : K{
    public:
    typedef typename K::result_type result_type;

    //Construction
    mono_bw_kernel();
    template<typename K1> mono_bw_kernel(K1 k1); //pass bandwith, or kernel
    
    // Evaluate
    template<typename X> result_type profile(const X& x)const;    
    template<typename X> result_type operator()(const X& x)const;    
    template<typename X,typename X1> 
    result_type operator()(const X& x,const X1& x1)const;
    
    // Access
    static unsigned dimension;
    result_type radius()const;
    result_type normalizing_constant()const;
    
    private:
    result_type normalizing_constant_; //no ambiguity because inherit K privately
    result_type comp_nc()const; 
};

// Construction
template<typename K,unsigned M> 
mono_bw_kernel<K,M>::mono_bw_kernel():K(),normalizing_constant_(comp_nc()){}

template<typename K,unsigned M> 
template<typename K1>
mono_bw_kernel<K,M>::mono_bw_kernel(K1 k1):K(k1),normalizing_constant_(comp_nc()){}
        
// Evaluate
template<typename K,unsigned M> 
template<typename X> 
typename mono_bw_kernel<K,M>::result_type 
mono_bw_kernel<K,M>::profile(const X& x)const{
    static result_type init = static_cast<result_type>(0);
    const K& k = static_cast<const K&>(*this);
    result_type norm = std::accumulate(
        boost::begin(x),
        boost::end(x),
        init,
        ( lambda::_1 + (lambda::_2 * lambda::_2 ) )
    );   
    norm = std::sqrt(norm);
    return k.profile(norm);
}

template<typename K,unsigned M> 
template<typename X> 
typename mono_bw_kernel<K,M>::result_type 
mono_bw_kernel<K,M>::operator()(const X& x)const{
    return this->profile(x) / this->normalizing_constant();
}

template<typename K,unsigned M> 
template<typename X,typename X1> 
typename mono_bw_kernel<K,M>::result_type 
mono_bw_kernel<K,M>::operator()(const X& x,const X1& x1)const{
    typedef vector_space::lazy_difference<X,X1> diff_;
    typedef typename range_size<X>::type size_type;
    BOOST_ASSERT(size(x) == static_cast<size_type>(size(x1)));
    BOOST_ASSERT(size(x) == static_cast<size_type>(M));
    diff_ diff(x,x1);
    return (*this)(diff);
}
        
// Access
template<typename K,unsigned M> unsigned mono_bw_kernel<K,M>::dimension = M;

template<typename K,unsigned M>
typename mono_bw_kernel<K,M>::result_type 
mono_bw_kernel<K,M>::radius()const{ 
    const K& k = static_cast<const K&>(*this);
    return k.radius();
}

template<typename K,unsigned M>
typename mono_bw_kernel<K,M>::result_type 
mono_bw_kernel<K,M>::normalizing_constant()const{ return normalizing_constant_; }
    
template<typename K,unsigned M>
typename mono_bw_kernel<K,M>::result_type 
mono_bw_kernel<K,M>::comp_nc()const{
    const K& k = static_cast<const K&>(*this);
    static result_type m = static_cast<result_type>(M);
    result_type nc = k.normalizing_constant();
    return std::pow(nc,m);
}
 
}// multivariate
}// kernel
}// detail
}// statistics
}// boost   

#endif