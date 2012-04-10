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

/*! \file IfElse.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "functions/IfElse.hpp"

namespace Biips
{

  Bool IfElse::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    DimArray ref_dim;

    for (Size i = 0; i < paramDims.size(); ++i)
    {
      if (!paramDims[i]->IsScalar())
      {
        ref_dim = *paramDims[i];
        break;
      }
    }

    for (Size i = 0; i < paramDims.size(); ++i)
    {
      if (paramDims[i]->IsScalar())
        continue;
      if (*paramDims[i] != ref_dim)
        return false;
    }
    return true;
  }

  DimArray IfElse::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    for (Size i = 0; i < paramDims.size(); ++i)
    {
      if (!paramDims[i]->IsScalar())
        return *paramDims[i];
    }

    return *paramDims[0];
  }

  void IfElse::eval(ValArray & values,const NumArray::Array & paramValues) const
  {
    const NumArray & x = paramValues[0];
    const NumArray & a = paramValues[1];
    const NumArray & b = paramValues[2];

    for (Size i = 0, j = 0, k = 0, l = 0; i < values.size(); ++i)
    {
      values[i] = x.Values()[j] ? a.Values()[k] : b.Values()[l];

      if (!x.IsScalar())
        ++j;
      if (!a.IsScalar())
        ++k;
      if (!b.IsScalar())
        ++l;
    }
  }

  Bool IfElse::IsDiscreteValued(const Flags & mask) const
  {
    return mask[1] && mask[2];
  }
}
