///////////////////////////////////////////////////////////////////////////////
// vector_space::data::lazy_difference.hpp                                   //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
/////////////////////////////////////////////////////////////////////////////// 
#ifndef BOOST_STATISTICS_DETAIL_VECTOR_SPACE_DATA_LAZY_DIFFERENCE_H_ER_2009
#define BOOST_STATISTICS_DETAIL_VECTOR_SPACE_DATA_LAZY_DIFFERENCE_H_ER_2009
#include <boost/mpl/not.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/type_traits.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/range.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/statistics/detail/tuple/functor/element.hpp>
#include <boost/vector_space/data/detail/cref.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace vector_space{

// A data structure that offers a lazy (i.e. no copying involved) representation 
// of an element by element difference between two ranges.
//
// Usage:
// typedef lazy_difference<X,X1> diff_;
// typedef range_value<diff_> val_;
// diff_ diff(x,x1);
// val d0 = const_begin(diff)
template<
    typename X,
    typename X1,
    typename T = typename remove_cv<typename range_value<X>::type>::type
>
class lazy_difference{

    typedef T value_type;

    typedef vector_space::detail::template cref<X> cref_x_;
    typedef vector_space::detail::template cref<X1> cref_x1_;       
    typedef typename cref_x_::const_iterator it_x_;
    typedef typename cref_x1_::const_iterator it_x1_;
    typedef boost::tuple<it_x_,it_x1_> it_tuple_;
    typedef boost::zip_iterator<it_tuple_> zip_it_;
    // Constructed on the fly so it is not a reference
    typedef typename boost::iterator_reference<zip_it_>::type ref_; 
    // This is a const ref to a tuple
    typedef typename add_reference<
        typename add_const<ref_>::type
    >::type ref2_; 
    typedef boost::function<value_type(ref2_)> f_;
    
    public:
    lazy_difference(const X& x,const X1& x1)
    :cref_x(x),cref_x1(x1){}
    
    lazy_difference(const lazy_difference& that)
    :cref_x(that.cref_x),cref_x1(that.cref_x1){}

    typedef boost::transform_iterator<f_,zip_it_> const_iterator;
    typedef const_iterator iterator;

    const_iterator begin()const{
        return this->get_iterator(
            cref_x.begin(),
            cref_x1.begin()
        );
    }
    const_iterator end()const{
        return this->get_iterator(
            cref_x.end(),
            cref_x1.end()
        );
    }
    
    //private:
    cref_x_ cref_x;
    cref_x1_ cref_x1;

    lazy_difference& operator=(const lazy_difference&); // disabled
    
    const_iterator get_iterator(it_x_ it_x,it_x1_ it_x1)const{
        typedef  boost::statistics::detail::tuple::functor::element<0> f0_;
        typedef  typename boost::result_of<f0_(ref2_)>::type r0_;
        typedef  boost::statistics::detail::tuple::functor::element<1> f1_;
        typedef  typename boost::result_of<f1_(ref2_)>::type r1_;
        typedef boost::function<T(r0_,r1_)> min_;
        
        min_ min = boost::lambda::_1 - boost::lambda::_2;
        
        f_ f = boost::lambda::bind<T>(
            min,
            boost::lambda::bind<r0_>(f0_(),boost::lambda::_1),
            boost::lambda::bind<r1_>(f1_(),boost::lambda::_1)
        );

        return  make_transform_iterator(
            zip_it_(
                make_tuple(
                    it_x,
                    it_x1
                )
            ),
            f
        );
    }
    
};

}// vector_space
}// detail
}// statistics
}// boost

#endif