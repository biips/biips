///////////////////////////////////////////////////////////////////////////////
// kernel::scalar::crtp.hpp                                                  //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
/////////////////////////////////////////////////////////////////////////////// 
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_KERNELS_SCALAR_CRTP_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_KERNEL_KERNELS_SCALAR_CRTP_HPP_ER_2009
#include <boost/format.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{
namespace scalar{

// Overview: provides common operations for a kernel whose implementation
// is specified by a derived class, D.
//
// Let d note an instance of D, and T == D::result_type
//
// Requirements
// crtp<D,T> is a base of D
// D::result_type is defined
// D::core_profile(const T&)        returns an object of type T
// D::core_nc                       returns an object of type T (normalzing c)
// D d;                             Constructor Forwards to crtp<D,T>
// D d(r)                           Constructor Forwards to crtp<D,T>
template<typename D,typename T> 
class crtp{
    public:
    typedef T result_type;

    // Construction
    crtp();
    crtp(const result_type& bandwidth);
    
    // Evaluate
    template<typename X> result_type profile(const X& x)const;
    template<typename X> result_type operator()(const X& x)const;
    template<typename X,typename X1> 
    result_type operator()(const X& x,const X1& x1)const;
    
    // Access
    result_type bandwidth()const;
    result_type normalizing_constant()const;
    
    private:
    result_type bandwidth_;
    result_type normalizing_constant_;
    result_type comp_nc(result_type bandwidth);
};

template<typename D,typename T>
std::ostream& operator<<(std::ostream& out,const crtp<D,T>& k){
    const char* str = "(%1%,%2%)";
    format f(str); f%k.bandwidth()%k.normalizing_constant();
    out << f.str();
    return out; 
}

// Construction
template<typename D,typename T> crtp<D,T>::crtp()
:bandwidth_(static_cast<result_type>(1)),
normalizing_constant_(this->comp_nc(bandwidth())){}

template<typename D,typename T> crtp<D,T>::crtp(const result_type& bandwidth)
:bandwidth_(bandwidth),
normalizing_constant_(this->comp_nc(this->bandwidth())){}


// Evaluate
template<typename D,typename T>    
template<typename X>
typename crtp<D,T>::result_type 
crtp<D,T>::profile(const X& x)const{
    result_type u = x / this->bandwidth();
    return D::core_profile(u);
} 

template<typename D,typename T>    
template<typename X>
typename crtp<D,T>::result_type 
crtp<D,T>::operator()(const X& x)const{
    return ( this->profile(x) ) / ( this->normalizing_constant()) ;
} 

template<typename D,typename T>
template<typename X,typename X1> 
typename crtp<D,T>::result_type 
crtp<D,T>::operator()(const X& x,const X1& x1)const{
    return (*this)(x-x1);
}
  
// Access
template<typename D,typename T>    
typename crtp<D,T>::result_type crtp<D,T>::bandwidth()const{ 
    return this->bandwidth_; 
}

template<typename D,typename T>    
typename crtp<D,T>::result_type crtp<D,T>::normalizing_constant()const{ 
    return this->normalizing_constant_; 
}

// Implem
template<typename D,typename T>    
typename crtp<D,T>::result_type crtp<D,T>::comp_nc(result_type bandwidth){ 
    return D::core_nc * bandwidth;  
}

}// scalar
}// kernel
}// detail
}// statistics
}// boost

#endif