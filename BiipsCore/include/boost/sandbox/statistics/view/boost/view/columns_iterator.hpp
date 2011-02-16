///////////////////////////////////////////////////////////////////////////////
// view::iterator::columns_iterator.hpp                                      //
//                                                                           //
//  (C) Copyright 2009 Erwann Rogard                                         //
//  Use, modification and distribution are subject to the                    //
//  Boost Software License, Version 1.0. (See accompanying file              //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_VIEW_DETAIL_COLUMNS_ITERATOR_HPP_ER_2009
#define BOOST_VIEW_DETAIL_COLUMNS_ITERATOR_HPP_ER_2009
#include <iterator>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <boost/view//step_count_down_iterator.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/range.hpp>

namespace boost{
namespace view{
namespace detail{

// I is an iterator to some sequence
// 
// *columns_iterator returns a range representing a columns in  
// row-major matrix view. Each row is of length step 
template< class I
        , class V = use_default
        , class C = use_default
        , class R = use_default
        , class D = use_default 
>
class columns_iterator
  : public iterator_adaptor< 
        columns_iterator<I, V, C, R, D>, 
        I, 
        iterator_range<
            step_count_down_iterator<I,V,C,R,D>           //V1
        >,                                     
        C, 
        iterator_range<
            step_count_down_iterator<I,V,C,R,D>           // R1
        >, 
        D 
    >
{
    typedef iterator_adaptor< 
        columns_iterator<I, V, C, R, D>, 
            I, 
            iterator_range<
            step_count_down_iterator<I,V,C,R,D>           
        >,                                     
        C, 
        iterator_range<
            step_count_down_iterator<I,V,C,R,D>           
        >, 
        D 
  > super_;

    typedef step_count_down_iterator<I,V,C,R,D> iter_;
    typedef iterator_range<iter_> range_;

 public:
    typedef typename super_::difference_type difference_type; 
    typedef typename super_::reference reference; 

    columns_iterator() : super_(){}

    explicit columns_iterator(  
       I b,
       I e,
       difference_type step
    )
    :super_(b),base_e_(e),step_(step){
        BOOST_ASSERT(
            this->step()>=0
        );
    }
    
    columns_iterator(const columns_iterator& that)
    :super_(that),base_e_(that.base_e_),step_(that.step_){}
    
    difference_type step()const{ return this->step_; }
    
 private:

    friend class iterator_core_access;

    // default should work for:
    // void increment();
    // void decrement()
    // void advance(difference_type n)
    // difference_type distance_to(columns_iterator const& other) const{
    // bool equal(columns_iterator const& other) const

    reference dereference() const {
        // TODO may check against end
        iter_ b = make_step_count_down_iterator(
            this->base(),
            this->base_e_,
            this->step()
        );
        iter_ e = make_end_step_count_down_iterator(
            this->base(),
            this->base_e_,
            this->step()
        );
        return range_(
            b,
            e
        );
    }

    void advance(difference_type n)
    {
        std::advance(this->base_reference(), n);
    }
    I base_e_;
    difference_type step_;
};


template<typename It,typename N>
columns_iterator<It>
make_columns_iterator(
    It b,
    It e,
    N stride
){
    typedef columns_iterator<It> it_;
    return it_(
        b,
        e,
        stride
    );
}


// [b,e) is the sequence from which the matrix view is created
template<typename It,typename N>
columns_iterator<It>
make_end_columns_iterator(
    It b,
    It e,
    N step
){
    typedef columns_iterator<It> it_;
    return it_(
        boost::next(b,step),
        e,
        step
    );
}

}// detail
}// view
}// boost

#endif



