//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file Vector.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_VECTOR_HPP_
#define BIIPS_VECTOR_HPP_

#include "common/NumArray.hpp"
#include "common/MultiArray.hpp"
#include <boost/numeric/ublas/vector.hpp>
//#include <boost/numeric/ublas/vector_proxy.hpp>

namespace Biips
{

  class Vector: public ublas::vector<NumArray::ValueType, NumArray::StorageType>
  {
  public:
    typedef ublas::vector<NumArray::ValueType, NumArray::StorageType> BaseType;

    Vector()
    {
    }
    explicit Vector(size_type sz) :
        BaseType(sz)
    {
    }
    Vector(size_type sz, value_type val) :
        BaseType(sz, array_type(sz, val))
    {
    }
    Vector(size_type sz, const array_type & value) :
        BaseType(sz, value)
    {
    }
    explicit Vector(const NumArray & data);
    explicit Vector(const MultiArray & data);
    template<class AE>
    Vector(const ublas::vector_expression<AE> &ae) :
        BaseType(ae)
    {
    }

    template<class AE>
    Vector & operator =(const ublas::vector_expression<AE> & ae)
    {
      operator=(Vector(ae));
      return *this;
    }
  };

  class VectorRef: public ublas::vector<NumArray::ValueType,
      NumArray::StorageType>
  {
  public:
    typedef NumArray::ValueType ValueType;
    typedef NumArray::StorageType StorageType;
    typedef ublas::vector<ValueType, StorageType> BaseType;

  protected:
    DimArray * pDim_;
    StorageType * pValues_;
    mutable Bool released_;

    using BaseType::swap;

  public:
    explicit VectorRef(NumArray & dat) : BaseType(dat.Length(), array_type()),
    pDim_(dat.DimPtr()),
    pValues_(dat.ValuesPtr()),
    released_(false)
    {
      BaseType::data().swap(dat.Values());
    }
    explicit VectorRef(MultiArray & dat) : BaseType(dat.Length(), array_type()),
    pDim_(dat.DimPtr().get()),
    pValues_(dat.ValuesPtr().get()),
    released_(false)
    {
      BaseType::data().swap(dat.Values());
    }

    VectorRef(VectorRef & vec_ref);

    VectorRef(const VectorRef & vec_ref);

    template<class AE>
    VectorRef & operator = (const ublas::vector_expression<AE> & ae)
    {
      BaseType::operator=(ae);
      return *this;
    }

    virtual ~VectorRef()
    {
      Release();
    }

    void Release();

    Bool Released() const
    {
      return released_;
    }
  };

}

#endif /* BIIPS_VECTOR_HPP_ */
