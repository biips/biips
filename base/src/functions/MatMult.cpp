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

/*! \file MatMult.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from JAGS MatMult.cc
 */

#include "functions/MatMult.hpp"

namespace Biips
{

  Bool MatMult::checkParamDims(
      const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];
    if ((left.size() > 2) || (right.size() > 2))
      return false;
    if (left.IsVector())
      return (left[0] == right[0]);
    else
      return (left[1] == right[0]);
  }

  void MatMult::eval(ValArray & values,
                     const NumArray::Array & paramValues) const
  {
    const NumArray & left = paramValues[0];
    const NumArray & right = paramValues[1];


    // FIXME: this is only valid for column major order
    Size d1, d2, d3;

    if (left.IsVector())
    {
      d1 = 1;
      d2 = left.Dim()[0];
    }
    else
    {
      d1 = left.Dim()[0];
      d2 = left.Dim()[1];
    }
    if (right.IsVector())
      d3 = 1;
    else
      d3 = right.Dim()[1];

    for (Size i = 0; i < d1; ++i)
    {
      for (Size j = 0; j < d3; ++j)
      {
        values[i + d1 * j] = 0;
        for (Size k = 0; k < d2; ++k)
        {
          values[i + d1 * j] += left.Values()[i + d1 * k]
                                * right.Values()[k + d2 * j];
        }
      }
    }

  }

  DimArray MatMult::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];

    DimArray dim(2, 1);
    if (left.IsMatrix())
      dim.push_back(left[0]);
    if (right.IsMatrix())
      dim.push_back(right[1]);
    return dim.Drop();
  }

  Bool MatMult::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    return (knownMask[0] && scaleMask[1]) || (scaleMask[0] && knownMask[1]);
  }
}
