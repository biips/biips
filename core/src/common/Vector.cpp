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

/*! \file Vector.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "common/Vector.hpp"

namespace Biips
{

  Vector::Vector(const NumArray & data) :
    Vector::BaseType(data.Dim()[0], data.Values())
  {
    if (data.NDim() > 1)
      throw LogicError("Can not construct Vector: NumArray has more than 1 dimension.");
  }
  Vector::Vector(const MultiArray & data) :
    Vector::BaseType(data.Dim()[0], data.Values())
  {
    if (data.NDim() > 1)
      throw LogicError("Can not construct Vector: MultiArray has more than 1 dimension.");
  }

  VectorRef::VectorRef(VectorRef & vec_ref) :
    VectorRef::BaseType(vec_ref.size(), array_type()), pDim_(vec_ref.pDim_), pValues_(vec_ref.pValues_), released_(vec_ref.released_)
  {
    if (!released_)
    {
      swap(vec_ref);
      vec_ref.released_ = true;
    }
  }

  VectorRef::VectorRef(const VectorRef & vec_ref) :
    VectorRef::BaseType(vec_ref), pDim_(vec_ref.pDim_), pValues_(vec_ref.pValues_), released_(vec_ref.released_)
  {
    if (!released_)
    {
      vec_ref.released_ = true;
    }
  }

  void VectorRef::Release()
  {
    if (!released_)
    {
      (*pDim_)[0] = size();
      data().swap(*pValues_);
      released_ = true;
    }
  }

}

