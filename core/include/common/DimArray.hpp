//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
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

/*! \file DimArray.hpp
 * \brief A dimensions array class
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DIMARRAY_HPP_
#define BIIPS_DIMARRAY_HPP_

#include "common/Types.hpp"

#include <numeric>
#include <functional>

namespace Biips
{
  //! A dimensions array class
  /*!
   * DimArray is a container of the sizes of each dimension of a N-dimensional object.
   * It inherits publicly std::vector<Size>, adding some convenience
   * semantic methods.
   */
  class DimArray: public Types<Size>::Array
  {
  public:
    typedef Types<Size>::Array BaseType;
    typedef DimArray SelfType;
    //! A shared pointer of DimArray
    typedef Types<SelfType>::Ptr Ptr;

    /*!
     * Creates an empty DimArray.
     */
    DimArray()
    {
    }
    /*!
     * Creates a DimArray with n elements.
     * \param n number of dimensions
     */
    explicit DimArray(size_type n) :
      BaseType(n)
    {
    }
    /*!
     * Creates a DimArray with n copies of s.
     * \param n number of dimensions
     * \param s size of the dimensions
     */
    DimArray(size_type n, value_type s) :
      BaseType(n, s)
    {
    }
    /*!
     * Creates a DimArray from a copy of a range.
     * Will copy the sequence from first to last excluded.
     * \param first iterator on the first element of the range
     * \param last iterator on the last element of the range
     */
    template<class InputIterator>
    DimArray(InputIterator first, InputIterator last) :
      BaseType(first, last)
    {
    }

    //! The assignment operator
    DimArray & operator=(const DimArray & from)
    {
      BaseType::operator=(from);
      return *this;
    }
    ;

    //! Number of dimensions accessor
    /*!
     * @return the size of the DimArray
     */
    size_type NDim() const
    {
      return size();
    }
    //! Length of the DimArray
    /*!
     * Computes the product of the sizes of each dimension
     * @return the number of elements contained in an array with such DimArray
     */
    value_type Length() const
    {
      return empty() ? 0 : std::accumulate(begin(),
                                           end(),
                                           value_type(1),
                                           std::multiplies<value_type>());
    }
    /*!
     * Indicates whether the DimArray corresponds to a scalar object,
     * i.e. containing only one element.
     */
    Bool IsScalar() const
    {
      return (Length() == 1);
    } // FIXME
    /*!
     * Indicates whether the DimArray corresponds to a vector object,
     * i.e. only one dimension.
     */
    Bool IsVector() const
    {
      return (size() == 1);
    }
    /*!
     * Indicates whether the DimArray corresponds to a matrix object,
     * i.e. exactly two dimensions.
     */
    Bool IsMatrix() const
    {
      return (size() == 2);
    }
    /*!
     * Indicates whether the DimArray corresponds to a squared matrix.
     */
    Bool IsSquared() const
    {
      return (IsMatrix() && (at(0) == at(1)));
    }

    DimArray Drop() const;
  };

  const DimArray::Ptr P_SCALAR_DIM(new DimArray(1, 1));
}

#endif /* BIIIPS_DIMARRAY_HPP_ */
