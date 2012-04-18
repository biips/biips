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

/*! \file Matrix.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MATRIX_HPP_
#define BIIPS_MATRIX_HPP_

#include "common/MultiArray.hpp"

#include <boost/numeric/ublas/matrix.hpp>

namespace Biips
{

  class Matrix: public ublas::matrix<NumArray::ValueType,
      NumArray::StorageOrderType, NumArray::StorageType>
  {
  public:
    typedef ublas::matrix<NumArray::ValueType, NumArray::StorageOrderType,
        NumArray::StorageType> BaseType;

    Matrix()
    {
    }
    Matrix(size_type sz1, size_type sz2) :
      BaseType(sz1, sz2)
    {
    }
    Matrix(size_type sz1, size_type sz2, value_type val) :
      BaseType(sz1, sz2, array_type(sz1 * sz2, val))
    {
    }
    Matrix(size_type sz1, size_type sz2, const array_type & value) :
      BaseType(sz1, sz2, value)
    {
    }
    explicit Matrix(const NumArray & data) :
      BaseType(data.Dim()[0], data.Dim()[1], data.Values())
    {
    }
    template<class AE>
    Matrix(const ublas::matrix_expression<AE> &ae) :
      BaseType(ae)
    {
    }

    template<class AE>
    Matrix & operator =(const ublas::matrix_expression<AE> & ae)
    {
      BaseType::operator=(ae);
      return *this;
    }
  };

  class MatrixRef: public ublas::matrix<NumArray::ValueType,
      NumArray::StorageOrderType, NumArray::StorageType>
  {
  public:
    typedef NumArray::ValueType ValueType;
    typedef NumArray::StorageOrderType StorageOrder;
    typedef NumArray::StorageType StorageType;
    typedef ublas::matrix<ValueType, StorageOrder, StorageType> BaseType;

  protected:
    NumArray * pData_;
    mutable Bool released_;

    BaseType::swap;

  public:
    explicit MatrixRef(NumArray & dat) : BaseType(dat.Dim()[0], dat.Dim()[1], array_type()), pData_(&dat), released_(false)
    { BaseType::data().swap(pData_->Values());}

    MatrixRef(MatrixRef & mat_ref) : BaseType(mat_ref.size1(), mat_ref.size2(), array_type()), pData_(mat_ref.pData_), released_(mat_ref.released_)
    {
      if ( !released_ )
      {
        swap(mat_ref);
        mat_ref.released_ = true;
      }
    }

    MatrixRef(const MatrixRef & mat_ref) : BaseType(mat_ref), pData_(mat_ref.pData_), released_(mat_ref.released_)
    {
      if ( !released_ )
      mat_ref.released_ = true;
    }

    template<class AE>
    MatrixRef & operator = (const ublas::matrix_expression<AE> & ae)
    { BaseType::operator=(ae); pData_->Dim()[0] = size1(); pData_->Dim()[1] = size2(); return *this;}

    // TODO put in cpp
    void resize (size_type size_1, size_type size_2, bool preserve = true)
    { BaseType::resize(size_1, size_2, preserve); pData_->Dim()[0] = size_1; pData_->Dim()[1] = size_2;}

    virtual ~MatrixRef()
    { if (! released_) data().swap(pData_->Values());}

    void Release()
    { if (! released_)
      { data().swap(pData_->Values()); released_ = true;}}

    Bool Released() const
    { return released_;}
  };

}

#endif /* BIIPS_MATRIX_HPP_ */