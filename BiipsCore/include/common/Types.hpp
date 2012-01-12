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
#include <cmath>
#include <limits>
#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/fwd.hpp>

//! The namespace where all @biips symbols live.
namespace Biips
{
  //! Character string type
  typedef std::string String;

  //! Boolean type
  typedef bool Bool;

  typedef std::vector<Bool> Flags;

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
  template<typename T>
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
    typedef typename Array::const_iterator ConstIterator; //!< Const iterator type of the Array type

    typedef std::vector<Ptr> PtrArray; //!< Array of pointers defined with a std::vector<boost::shared_ptr<> >

    typedef std::pair<SelfType, SelfType> Pair; //!< Pair type consisting of two elements of the same type
    typedef std::pair<Iterator, Iterator> IteratorPair; //!< Pair of Iterators type, used to define a range
    typedef std::pair<ConstIterator, ConstIterator> ConstIteratorPair; //!< Pair of ConstIterators type, used to define a range
  };

  //! Node identifiers type
  /*!
   * Each Node in a Graph is associated a NodeId, i.e. a unique identifer
   * which is nothing but a positive integer.
   * It is used as index to access the Node in a Graph object or its
   * associated properties in arrays.
   */
  typedef Size NodeId;

  //! Alias for ublas namespace
  /*!
   * Boost.uBLAS is a C++ template class library for Basic Linear Algebra.
   * It provides BLAS level 1, 2, 3 functionality for dense, packed and sparse matrices.
   * The design and implementation unify mathematical notation via operator overloading
   * and efficient code generation via expression templates.
   * http://www.boost.org/doc/libs/1_44_0/libs/numeric/ublas/doc/index.htm
   */
  namespace ublas = boost::numeric::ublas;

  //! The row-major ordering type
  /*!
   * Specifies the storage ordering type of the values of an
   * n-dimensional object in a contiguous array container.
   * Row-major means the right most index moves faster.
   * Row-major is used in C/C++ arrays.
   */
  typedef ublas::row_major RowMajorOrder;
  //! The column-major ordering type
  /*!
   * Specifies the ordering type of values of an n-dimensional
   * in a contiguous array container.
   * Column-major means the left most index moves faster.
   * Column-major is used in Fortran, Matlab, Octave and R arrays.
   */
  typedef ublas::column_major ColumnMajorOrder;

  //! Storage order used in Biips is column-major (ensures compatibility with R and Matlab)
  typedef ColumnMajorOrder StorageOrder;

  const Size BIIPS_SIZEMIN = std::numeric_limits<Size>::min();
  const Size BIIPS_SIZEMAX = std::numeric_limits<Size>::max();
  const Size BIIPS_SIZENA = BIIPS_SIZEMAX - 1;

  const Int BIIPS_INTMIN = std::numeric_limits<Int>::min();
  const Int BIIPS_INTMAX = std::numeric_limits<Int>::max();

  const Real BIIPS_REALMIN = std::numeric_limits<Real>::min();
  const Real BIIPS_REALMAX = std::numeric_limits<Real>::max();
  const Real BIIPS_NEGINF = -std::numeric_limits<Real>::infinity();
  const Real BIIPS_POSINF = std::numeric_limits<Real>::infinity();
  const Real BIIPS_REALNAN = std::numeric_limits<Real>::quiet_NaN();
  const Real BIIPS_REALNA = BIIPS_REALMAX * (1
      - std::numeric_limits<Real>::epsilon());

  const NodeId NULL_NODEID = BIIPS_SIZENA;

  inline Bool isFinite(Scalar r)
  {
    return finite(r);
  }

  inline Bool isNan(Scalar r)
  {
    return isnan(r);
  }
  inline Bool isNan(Size s)
  {
    return isnan(s);
  }

}

#endif /* BIIPS_TYPES_HPP_ */
