///////////////////////////////////////////////////////////////////////////////
// importance_sampling::grid.hpp                                             //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_GRID_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_GRID_HPP_ER_2009
#include <boost/numeric/conversion/converter.hpp>
#include <boost/limits.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace importance_sampling{

// Equally spaced univariate grid.
template<typename T>
class grid{
    public:
    typedef std::size_t size_type;
    private:
    typedef boost::numeric::converter<T,size_type> conv_t;
    public:
    typedef T result_type;
    grid(T min, T max, size_type n);
    T operator()();
        
    private:
    size_type   n_;
    size_type   i_;
    T           d_;
    T           x_;
};

template<typename T>
grid<T>::grid(T min, T max, size_type n) 
: n_(n), i_(0), d_((max-min)/conv_t::convert(n-1)),x_(min) {}
        
template<typename T>
T grid<T>::operator()(){
    BOOST_ASSERT(i_<n_);
    T result = x_;
    x_ += d_;
    ++i_;
    return result;
}

}// importance_sampling
}// detail
}// statistics
}// boost

#endif 