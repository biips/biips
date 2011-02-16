///////////////////////////////////////////////////////////////////////////////
// view::iterator::rows_iterator.hpp                                         //
//                                                                           //
//  (C) Copyright 2009 Erwann Rogard                                         //
//  Use, modification and distribution are subject to the                    //
//  Boost Software License, Version 1.0. (See accompanying file              //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_VIEW_ITERATOR_ROWS_ITERATOR_HPP_ER_2009
#define BOOST_VIEW_ITERATOR_ROWS_ITERATOR_HPP_ER_2009
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/range.hpp>
#include <boost/view//step_count_down_iterator.hpp>

namespace boost{
namespace view{
namespace detail{

// I is an iterator type to some sequence
//
// *rows_iterator returns a range representing a row in a 
// row-major matrix view. Each row is of length step
template< class I
        , class V = use_default
        , class C = use_default
        , class R = use_default
        , class D = use_default 
>
class rows_iterator
  : public iterator_adaptor< 
             rows_iterator<I, V, C, R, D>, 
             step_count_down_iterator<I,V,C,R,D>, 
             iterator_range<I>,                                 //V1
             C, 
             iterator_range<I>,                                 // R1
             D 
    >
{
  typedef iterator_adaptor< 
        rows_iterator<I, V, C, R, D>, 
        step_count_down_iterator<I,V,C,R,D>, 
        iterator_range<I>, // V1                                     
        C, 
        iterator_range<I>, // R1 
        D 
    > super_;

    typedef step_count_down_iterator<I,V,C,R,D> iter_;
    typedef iterator_range<I> range_;

 public:
    typedef typename super_::difference_type difference_type; 
    typedef typename super_::reference reference; 

    rows_iterator() : super_(){}

    explicit rows_iterator(  
       I b,
       I e,
       difference_type step
    )
    :super_(
        iter_(b,e,step)
    ),
    base_e_(e),
    step_(step){}
    
    rows_iterator(const rows_iterator& that)
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
        if(this->base().count_down()==0){
            throw std::out_of_range(
                "rows_iterator::dereference at one past the end"
            );
        }
        
        I b = (this->base()).base();
        I e = boost::next(b,this->step());
        // This would be wrong because step_count_down_iterator uses a
        // modulo 1 base iterator as one-past the end.
        // I e = boost::next(this->base(),1).base(); 

        return range_(
            b,
            e
        );
    }

    I base_e_;
    difference_type step_;
};


template<typename It,typename N>
rows_iterator<It>
make_rows_iterator(
    It b,
    It e,
    N step
){
    typedef rows_iterator<It> it_;
    return it_(
        b,
        e,
        step
    );
}


// [b,e) is the sequence from which the matrix view is created
template<typename It,typename N>
rows_iterator<It>
make_end_rows_iterator(
    It b,
    It e,
    N step
){
    typedef rows_iterator<It> it_;
    return it_(
        make_end_step_count_down_iterator(b,e,step).base(),
        make_end_step_count_down_iterator(b,e,step).base(),
        step
    );
}

}// detail
}// view
}// boost

#endif



