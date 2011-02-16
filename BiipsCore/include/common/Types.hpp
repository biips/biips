//                                               -*- C++ -*-
/*! \file Types.hpp
 * \brief Defines the base types of BiiPS library.
 *
 * Most of them are typedefs.
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_TYPES_HPP_
#define BIIPS_TYPES_HPP_

#include <string>
#include <vector>
#include <deque>
//#include <limits>
#include <cmath>
#include <boost/shared_ptr.hpp>
#include <iostream>


//! The namespace where all @biips symbols live.
namespace Biips
{
  //! Character string type
  typedef std::string String;

  //! Boolean type
  typedef bool Bool;

  typedef std::deque<Bool> Flags;

  //! Positive integers type
  /*!
   * Used for sizes, dimensions, indices, ...
   */
  typedef unsigned int Size;

  //! Integer type
  typedef int Int;

  //! Real type
  typedef double Real;

  //! %Numerical scalar type
  typedef Real Scalar;


  //! Template structure defining usual derived types from the parameter type T
  template <typename T>
  struct Types
  {
    typedef T SelfType; //!< The template type itself

    //! Smart pointer type defined with a boost::shared_ptr<>
    /*!
     *  The shared_ptr class template stores a pointer to a dynamically allocated object,
     *  typically with a C++ new-expression. The object pointed to is guaranteed to be
     *  deleted when the last shared_ptr pointing to it is destroyed or reset.
     *  http://www.boost.org/doc/libs/1_44_0/libs/smart_ptr/shared_ptr.htm
     */
    typedef boost::shared_ptr<SelfType> Ptr;

    typedef std::vector<SelfType> Array; //!< Array type defined with a std::vector<>
    typedef typename Array::iterator Iterator; //!< Iterator type of the Array type
    typedef typename Array::reverse_iterator ReverseIterator; //!< Reverse iterator type of the Array type
    typedef typename Array::const_iterator ConstIterator; //!< Const iterator type of the Array type
    typedef typename Array::const_reverse_iterator ConstReverseIterator; //!< Const reverse iterator type of the Array type

    typedef std::vector<Ptr> PtrArray; //!< Array of pointers defined with a std::vector<boost::shared_ptr<> >

    typedef std::pair<SelfType, SelfType> Pair; //!< Pair type consisting of two elements of the same type
    typedef std::pair<Iterator, Iterator> IteratorPair; //!< Pair of Iterators type, used to define a range
    typedef std::pair<ConstIterator, ConstIterator> ConstIteratorPair; //!< Pair of ConstIterators type, used to define a range
  };


//  const Size BIIPS_SIZEMIN = std::numeric_limits<Size>::min();
//  const Size BIIPS_SIZEMAX = std::numeric_limits<Size>::max();
//
//  const Int BIIPS_INTMIN = std::numeric_limits<Int>::min();
//  const Int BIIPS_INTMAX = std::numeric_limits<Int>::max();
//
//  const Real BIIPS_REALMIN = std::numeric_limits<Real>::min();
//  const Real BIIPS_REALMAX = std::numeric_limits<Real>::max();
//  const Real BIIPS_NEGINF = -std::numeric_limits<Real>::infinity();
//  const Real BIIPS_POSINF = std::numeric_limits<Real>::infinity();
//  const Real BIIPS_NAN = std::numeric_limits<Real>::quiet_NaN();


//  Bool isFinite(Scalar r) { return finite(r); }
//
//  Bool isNan(Scalar r) { return isnan(r); }

}


#endif /* BIIPS_TYPES_HPP_ */
