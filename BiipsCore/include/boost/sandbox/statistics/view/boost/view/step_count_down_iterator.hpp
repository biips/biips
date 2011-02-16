// Copyright (C) 2003 Roland Richter <roland@flll.jku.at>
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//
// in Sept 2009, Erwann Rogard's modified version:
//  - this->foo() rather than foo() because compile errors with gcc 4.0/Mac OS
//  - deleted reachable functionality
//  - Uses a modulo 1 base iterator (e) to mark an end iterator
//    sequence =  {0,1,2,3,e}, step = 2;
//    offset:       base iterators:
//    0 :           {0,2,3}
//    1 :           {1,3,e}
//
// Warning : don't try step < 0 yet

#ifndef BOOST_VIEW_DETAIL_STEP_COUNT_DOWN_ITERATOR_HPP_ER_2009
#define BOOST_VIEW_DETAIL_STEP_COUNT_DOWN_ITERATOR_HPP_ER_2009

#include <iostream> // TODO remove
#include <string>
#include <stdexcept>
#include <boost/format.hpp>

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/next_prior.hpp>


namespace boost {
namespace view{
namespace detail{   // to avoid any conflict with existing files in view

template< class I
        , class V   = use_default
        , class C   = use_default
        , class R   = use_default
        , class D   = use_default 
>
class step_count_down_iterator
  : public iterator_adaptor< 
        step_count_down_iterator<I, V, C, R, D>, 
        I, 
        V, 
        C, 
        R, 
        D 
    >
{
    typedef std::string str_;
    typedef std::size_t size_;
public:

  typedef iterator_adaptor< 
        step_count_down_iterator<I, V, C, R, D>, 
        I, 
        V, 
        C, 
        R, 
        D 
  > super_type;

    typedef typename super_type::difference_type difference_type;
    typedef typename super_type::reference reference;

    step_count_down_iterator(): step_( 1 ),count_down_(0)
    {}
    
    // Constructs an iterator that traverses the subsequence
    // {(i+k), ..., (i+(n-1)k),(i+(n-1)k+1)}
    // where n is the largest number such that (i+(n-1)k+1)<=e
    explicit step_count_down_iterator( I i, I e, difference_type theStep = 1 )
    : super_type(i), step_( theStep ),count_down_(0)   
    {
        difference_type d = std::distance(i,e);
        if(d>0){
            this->count_down_ = (d-1)/this->step()+1;
        }
    
        BOOST_ASSERT( std::distance(i,e)>=0);
        BOOST_ASSERT( this->step() != 0 ); 
    }

    step_count_down_iterator( const step_count_down_iterator& rhs )
        : super_type( rhs ), step_( rhs.step_ ), count_down_(rhs.count_down_)
    {}

    //operator I() const
    //  { return this->base(); } 

    difference_type 
    step()const{ return this->step_ ;}

    difference_type count_down()const{ return this->count_down_; }

private:
    friend class iterator_core_access;

    // TODO
    /// If the step if negative behaves like a reverse_iterator
    reference dereference() const
    {
        //if( this->step() < 0 ) 
        //    return *boost::prior( this->base() );
        //else
        return *this->base();
    }

    void increment()
    {   
        if(this->count_down()==0){
            throw std::out_of_range(
                "view::detail::step_count_down_iterator"
            );
        }else{
            this->base_reference() = boost::next(
                this->base(),
                (this->count_down()>1)? this->step() : 1
            );
        }
        --this->count_down_;
    }

    void decrement()
    { 
        this->base_reference() = boost::prior(
            this->base(),
            (this->count_down() > 0) ? this->step() : 1
        );
        ++this->count_down_;
    } 

    void advance( difference_type n )
    {
        if(n==0){
            return;
        }
        difference_type n2 = this->count_down() - n;
        if(n2<0){
            throw std::out_of_range(
                "view::detail::step_count_down_iterator"
            );
        }
        if(n>0){
            this->base_reference() = boost::next(
                this->base(),
                (n2>0)
                    ?(n * this->step()):((n-1) * this->step()+1)
            ); 
        }else{
            this->base_reference() = boost::next(
                this->base(),
                (this->count_down()>0)
                    ?(n * this->step()):-1+(n+1) * this->step()
            ); 
        }
        this->count_down_ -= n; 
    }

    difference_type distance_to( const step_count_down_iterator& rhs ) const
    {
        BOOST_ASSERT(
            boost::next(
                *this,
                this->count_down()
            ) == boost::next(
                rhs,
                rhs.count_down()
            )
        );
        return this->count_down()-rhs.count_down();
    }

    bool equal( const step_count_down_iterator& rhs ) const
    {
        return( 
            this->base() == rhs.base() 
                && this->step() == rhs.step() ); 
    }

private: 
  difference_type step_;
  difference_type count_down_;
};

// Returns an iterator that traverses the subsequence
// {(i+k), ..., (i+(n-1)k),(i+(n-1)k+1)}
// where n is the largest number such that (i+(n-1)k+1)<=e
template< class I, class D >
inline step_count_down_iterator<I>
make_step_count_down_iterator( I i, I e, D k)
{
  typedef step_count_down_iterator<I> result_;
  return result_( i, e, k );
}

// Returns the step_iterator whose base is the maximum multiple number of steps 
// away from b, less or equal to e
template<typename I,typename D>
step_count_down_iterator<I>
make_end_step_count_down_iterator(I b,I e,D step){
    typedef typename iterator_difference<I>::type diff_;
    typedef step_count_down_iterator<I> res_;
    //TODO be able lift these restrictions
    BOOST_ASSERT(step>=0); 
    diff_ d = std::distance(b,e);
    BOOST_ASSERT(d>=0);
    if(d>0){
        d = (d-1)/step;
        b = boost::next(b,d * step+1);
        return res_(b,b,step);
    }else{
        return res_(e,e,step);
    }
}


}// detail
}// view
}// boost

#endif

