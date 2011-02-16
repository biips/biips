//                                               -*- C++ -*-
/*! \file ValArray.hpp
 * \brief A data values array class
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_VALARRAY_HPP_
#define BIIPS_VALARRAY_HPP_

#include <cassert>

#include "common/Types.hpp"

#include <boost/numeric/ublas/fwd.hpp>
#include <boost/operators.hpp>
#include <algorithm>

namespace Biips
{
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


  //! A values array class
  /*!
   * ValArray is the raw contiguous array container of data values.
   * It inherits publicly std::vector<Scalar>
   * and comes along with elementwise operators.
   */
  class ValArray : public std::vector<Scalar>,
  boost::arithmetic<ValArray,
  boost::arithmetic<ValArray, Scalar> >
  {
  public:
    typedef std::vector<Scalar> BaseType;
    typedef ValArray SelfType;
    //! A shared pointer of ValArray
    typedef Types<SelfType>::Ptr Ptr;

    //! Storage order used in Biips is row-major
    typedef RowMajorOrder StorageOrder;

    /*!
     * Creates an empty ValArray.
     */
    ValArray() {};
    /*!
     * Creates a ValArray with n elements.
     * @param n number of elements
     */
    explicit ValArray(size_type n) : BaseType(n) {};
    /*!
     * Creates a ValArray with n copies of val.
     * @param n number of elements
     * @param val value of the elements
     */
    ValArray(size_type n, value_type val) : BaseType(n, val) {}
    /*!
     * Creates a ValArray from a copy of a range.
     * Will copy the sequence from first to last excluded.
     * @param first iterator on the first element of the range
     * @param last iterator on the last element of the range
     */
    template<typename InputIterator>
    ValArray(InputIterator first, InputIterator last) : BaseType(first, last) {}

    //! Unary minus operator
    ValArray operator- () const;

    //! Unary logical not operator
    ValArray operator ! () const;

    //! Plus assign operator.
    /*!
     * Adds rhs elementwise.
     * @param rhs must have the same size
     */
    ValArray & operator += (const ValArray & rhs);
    //! Scalar plus assign operator.
    /*!
     *
     * Adds val to each element.
     */
    ValArray & operator += (Scalar val);
    //! Minus assign operator.
    /*!
     *
     * Substracts rhs elementwise.
     */
    ValArray & operator -= (const ValArray & rhs);
    //! Scalar minus assign operator.
    /*!
     *
     * Substracts val to each element.
     */
    ValArray & operator -= (Scalar val);
    //! Multiply assign operator.
    /*!
     *
     * Multiplies by rhs elementwise.
     * @param rhs must have the same size
     */
    ValArray & operator *= (const ValArray & rhs);
    //! Scalar multiply assign operator.
    /*!
     *
     * Multiplies each element by val.
     */
    ValArray & operator *= (Scalar val);
    //! Divide assign operator.
    /*!
     *
     * Divides by rhs elementwise.
     * @param rhs must have the same size
     */
    ValArray & operator /= (const ValArray & rhs);
    //! Scalar divide assign operator.
    /*!
     *
     * Divides each element by val.
     */
    ValArray & operator /= (Scalar val);

    ValArray operator == (const ValArray & rhs) const;
    ValArray operator != (const ValArray & rhs) const;
    ValArray operator > (const ValArray & rhs) const;
    ValArray operator < (const ValArray & rhs) const;
    ValArray operator >= (const ValArray & rhs) const;
    ValArray operator <= (const ValArray & rhs) const;

    ValArray operator == (Scalar val) const;
    ValArray operator != (Scalar val) const;
    ValArray operator > (Scalar val) const;
    ValArray operator < (Scalar val) const;
    ValArray operator >= (Scalar val) const;
    ValArray operator <= (Scalar val) const;

    ValArray operator && (const ValArray & rhs) const;
    ValArray operator || (const ValArray & rhs) const;

    ValArray operator && (Scalar val) const;
    ValArray operator || (Scalar val) const;


    //! Sum computation
    /*!
     * computes the sum of all elements
     */
    Scalar Sum() const;

    /*!
     * Most of the ValArray objects will contain one scalar value.
     * This method gives a convenient Scalar handle of the ValArray.
     * @return The first value of the array if size is 1
     */
    Scalar ScalarView() const { assert( size() == 1 ); return operator[](0); } // TODO throw exception
    /*!
     * Most of the ValArray objects will contain one scalar value.
     * This method gives a convenient Scalar handle of the ValArray.
     * @return A reference to the first value of the array if size is 1
     */
    Scalar & ScalarView() { assert( size() == 1 ); return operator[](0); } // TODO throw exception

    /*!
     * Applies an unary operator to each element of the ValArray.
     * @param op Unary operator. It can be a functor or a function pointer
     * @return The modified ValArray
     */
    template <typename UnaryOperator>
    ValArray Apply(UnaryOperator op) const;

    /*!
     * Applies a binary operator element-wise to two ValArray of the same size.
     * @param op Binary operator. It can be a functor or a function pointer
     * @param rhs The right hand side operand ValArray
     * @return The result ValArray
     */
    template <typename BinaryOperator>
    ValArray Apply(BinaryOperator op, const ValArray & rhs) const;

    /*!
     * Prints the elements of the ValArray in line and comma separated
     * @param os output stream receiving the print
     */
    void Print(std::ostream & os = std::cout) const;


  };


  template <typename UnaryOperator>
  ValArray ValArray::Apply(UnaryOperator op) const
  {
    ValArray ans(size());
    std::transform(begin(), end(), ans.begin(), op);
    return ans;
  }


  template <typename BinaryOperator>
  ValArray ValArray::Apply(BinaryOperator op, const ValArray & rhs) const
  {
    ValArray ans(size());
    std::transform(begin(), end(), rhs.begin(), ans.begin(), op);
    return ans;
  }


  ValArray operator - (Scalar val, const ValArray & rhs);
  ValArray operator / (Scalar val, const ValArray & rhs);

  ValArray operator == (Scalar val, const ValArray & rhs);
  ValArray operator != (Scalar val, const ValArray & rhs);
  ValArray operator > (Scalar val, const ValArray & rhs);
  ValArray operator < (Scalar val, const ValArray & rhs);
  ValArray operator >= (Scalar val, const ValArray & rhs);
  ValArray operator <= (Scalar val, const ValArray & rhs);

  inline Scalar sqrtScalar(Scalar s)
  {
    return std::sqrt(s);
  }


  inline Scalar cosScalar(Scalar s)
  {
    return std::cos(s);
  }


  struct PowScalar : public std::binary_function<Scalar, Scalar, Scalar>
  {
    Scalar operator() (Scalar base, Scalar exponent) const
    {
      return std::pow(base, exponent);
    }
  };

}


#endif /* BIIPS_VALARRAY_HPP_ */
