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

/*! \file   Matrix.cpp
 * \brief   
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "common/Matrix.hpp"

namespace Biips
{

  Matrix::Matrix(const NumArray & data) :
      Matrix::BaseType(data.Dim()[0],
                       data.IsVector() ? 1 : data.Dim()[1],
                       data.Values())
  {
    if (data.NDim() > 2)
      throw LogicError("Can not construct Matrix: NumArray has more than 2 dimensions.");
  }
  Matrix::Matrix(const MultiArray & data) :
      Matrix::BaseType(data.Dim()[0],
                       data.IsVector() ? 1 : data.Dim()[1],
                       data.Values())
  {
    if (data.NDim() > 2)
      throw LogicError("Can not construct Matrix: NumArray has more than 2 dimensions.");
  }

  MatrixRef::MatrixRef(MatrixRef & mat_ref) :
      MatrixRef::BaseType(mat_ref.size1(), mat_ref.size2(), array_type()), pDim_(mat_ref.pDim_), pValues_(mat_ref.pValues_), released_(mat_ref.released_)
  {
    if (!released_)
    {
      swap(mat_ref);
      mat_ref.released_ = true;
    }
  }

  MatrixRef::MatrixRef(const MatrixRef & mat_ref) :
      MatrixRef::BaseType(mat_ref), pDim_(mat_ref.pDim_), pValues_(mat_ref.pValues_), released_(mat_ref.released_)
  {
    if (!released_)
      mat_ref.released_ = true;
  }

  void MatrixRef::Release()
  {
    if (!released_)
    {
      (*pDim_)[0] = BaseType::size1();
      (*pDim_)[1] = BaseType::size2();
      data().swap(*pValues_);
      released_ = true;
    }
  }

}

