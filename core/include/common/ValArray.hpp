//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

#include "common/Types.hpp"

#include <boost/operators.hpp>
#include <algorithm>

namespace Biips
{

  //! A values array class
  /*!
   * ValArray is the raw contiguous array container of data values.
   * It inherits publicly std::vector<Scalar>
   * and comes along with element-wise operators.
   */
  class ValArray: public std::vector<Scalar>, boost::arithmetic<ValArray,
      boost::arithmetic<ValArray, Scalar> >
  {
  public:
    typedef std::vector<Scalar> BaseType;
    typedef ValArray SelfType;
    //! A shared pointer of ValArray
    typedef Types<SelfType>::Ptr Ptr;

    /*!
     * Creates an empty ValArray.
     */
    ValArray()
    {
    }

    /*!
     * Creates a ValArray with n elements.
     * @param n number of elements
     */
    explicit ValArray(size_type n) :
      BaseType(n)
    {
    }

    /*!
     * Creates a ValArray with n copies of val.
     * @param n number of elements
     * @param val value of the elements
     */
    ValArray(size_type n, value_type val) :
      BaseType(n, val)
    {
    }
    /*!
     * Creates a ValArray from a copy of a range.
     * Will copy the sequence from first to last excluded.
     * @param first iterator on the first element of the range
     * @param last iterator on the last element of the range
     */
    template<typename InputIterator>
    ValArray(InputIterator first, InputIterator last) :
      BaseType(first, last)
    {
    }

    //! Unary minus operator
    ValArray operator-() const;

    //! Unary logical not operator
    ValArray operator !() const;

    //! Plus assign operator.
    /*!
     * Adds rhs element-wise.
     * @param rhs must have the same size
     */
    ValArray & operator +=(const ValArray & rhs);
    //! Scalar plus assign operator.
    /*!
     *
     * Adds val to each element.
     */
    ValArray & operator +=(Scalar val);
    //! Minus assign operator.
    /*!
     *
     * Subtracts rhs element-wise.
     */
    ValArray & operator -=(const ValArray & rhs);
    //! Scalar minus assign operator.
    /*!
     *
     * Subtracts val to each element.
     */
    ValArray & operator -=(Scalar val);
    //! Multiply assign operator.
    /*!
     *
     * Multiplies by rhs element-wise.
     * @param rhs must have the same size
     */
    ValArray & operator *=(const ValArray & rhs);
    //! Scalar multiply assign operator.
    /*!
     *
     * Multiplies each element by val.
     */
    ValArray & operator *=(Scalar val);
    //! Divide assign operator.
    /*!
     *
     * Divides by rhs element-wise.
     * @param rhs must have the same size
     */
    ValArray & operator /=(const ValArray & rhs);
    //! Scalar divide assign operator.
    /*!
     *
     * Divides each element by val.
     */
    ValArray & operator /=(Scalar val);

    ValArray operator ==(const ValArray & rhs) const;
    ValArray operator !=(const ValArray & rhs) const;
    ValArray operator >(const ValArray & rhs) const;
    ValArray operator <(const ValArray & rhs) const;
    ValArray operator >=(const ValArray & rhs) const;
    ValArray operator <=(const ValArray & rhs) const;

    ValArray operator ==(Scalar val) const;
    ValArray operator !=(Scalar val) const;
    ValArray operator >(Scalar val) const;
    ValArray operator <(Scalar val) const;
    ValArray operator >=(Scalar val) const;
    ValArray operator <=(Scalar val) const;

    ValArray operator &&(const ValArray & rhs) const;
    ValArray operator ||(const ValArray & rhs) const;

    ValArray operator &&(Scalar val) const;
    ValArray operator ||(Scalar val) const;

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
    Scalar ScalarView() const
    {
      return operator[](0);
    } // TODO throw exception
    /*!
     * Most of the ValArray objects will contain one scalar value.
     * This method gives a convenient Scalar handle of the ValArray.
     * @return A reference to the first value of the array if size is 1
     */
    Scalar & ScalarView()
    {
      return operator[](0);
    } // TODO throw exception

    /*!
     * Applies an unary operator to each element of the ValArray.
     * @param op Unary operator. It can be a functor or a function pointer
     * @return The modified ValArray
     */
    template<typename UnaryOperator>
    ValArray Apply(UnaryOperator op) const;

    /*!
     * Applies a binary operator element-wise to two ValArray of the same size.
     * @param op Binary operator. It can be a functor or a function pointer
     * @param rhs The right hand side operand ValArray
     * @return The result ValArray
     */
    template<typename BinaryOperator>
    ValArray Apply(BinaryOperator op, const ValArray & rhs) const;

    /*!
     * Applies an unary operator to each element of this ValArray.
     * The result is stored in this ValArray.
     * @param op Unary operator. It can be a functor or a function pointer
     */
    template<typename UnaryOperator>
    ValArray & SelfApply(UnaryOperator op);

    /*!
     * Applies a binary operator element-wise to this ValArray and another
     * ValArray of the same size.
     * The result is stored in this ValArray.
     * @param op Binary operator. It can be a functor or a function pointer
     * @param rhs The right hand side operand ValArray
     * @return The result ValArray
     */
    template<typename BinaryOperator>
    ValArray & SelfApply(BinaryOperator op, const ValArray & rhs);

  };

  template<typename UnaryOperator>
  ValArray ValArray::Apply(UnaryOperator op) const
  {
    ValArray ans(size());
    std::transform(begin(), end(), ans.begin(), op);
    return ans;
  }

  template<typename BinaryOperator>
  ValArray ValArray::Apply(BinaryOperator op, const ValArray & rhs) const
  {
    ValArray ans(size());
    std::transform(begin(), end(), rhs.begin(), ans.begin(), op);
    return ans;
  }

  template<typename UnaryOperator>
  ValArray & ValArray::SelfApply(UnaryOperator op)
  {
    std::transform(begin(), end(), begin(), op);
    return *this;
  }

  template<typename BinaryOperator>
  ValArray & ValArray::SelfApply(BinaryOperator op, const ValArray & rhs)
  {
    std::transform(begin(), end(), rhs.begin(), begin(), op);
    return *this;
  }

  ValArray operator -(Scalar val, const ValArray & rhs);
  ValArray operator /(Scalar val, const ValArray & rhs);

  ValArray operator ==(Scalar val, const ValArray & rhs);
  ValArray operator !=(Scalar val, const ValArray & rhs);
  ValArray operator >(Scalar val, const ValArray & rhs);
  ValArray operator <(Scalar val, const ValArray & rhs);
  ValArray operator >=(Scalar val, const ValArray & rhs);
  ValArray operator <=(Scalar val, const ValArray & rhs);

}

namespace std
{
  Biips::ValArray sqrt(const Biips::ValArray & val);
}

#endif /* BIIPS_VALARRAY_HPP_ */
