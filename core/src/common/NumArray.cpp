//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
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

/*! \file   NumArray.cpp
 * \brief   
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "common/NumArray.hpp"
#include "common/MultiArray.hpp"
#include "common/Vector.hpp"
#include "common/Matrix.hpp"

namespace Biips
{

  NumArray::NumArray(const MultiArray & marray) :
      pDim_(marray.DimPtr().get()), pValues_(marray.ValuesPtr().get())
  {

  }

  Bool allMissing(const NumArray & marray)
  {
    Size N = marray.Length();
    const ValArray & v = marray.Values();
    for (Size i = 0; i < N; ++i)
    {
      if (v[i] != BIIPS_REALNA)
        return false;
    }
    return true;
  }

  Bool anyMissing(const NumArray & marray)
  {
    Size N = marray.Length();
    const ValArray & v = marray.Values();
    for (Size i = 0; i < N; ++i)
    {
      if (v[i] == BIIPS_REALNA)
        return true;
    }
    return false;
  }

  NumArrayArray::NumArrayArray(const MultiArrayArray & marray) :
      NumArrayArray::BaseType(marray.size())
  {
    for (Size i = 0; i < size(); ++i)
      at(i).SetPtr(marray[i].DimPtr().get(), marray[i].ValuesPtr().get());
  }

}
