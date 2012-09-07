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

/*! \file ArithmeticOperators.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */
#include "functions/ArithmeticOperators.hpp"

namespace Biips
{

  Bool Add::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }

  Bool Add::IsLinear(const Flags & linearMask, const Flags & knownMask) const
  {
    Size n_lin = 0;
    for (Size i = 0; i < linearMask.size(); ++i)
    {
      n_lin += linearMask[i];
      if (!(linearMask[i] || knownMask[i]))
        return false;
    }

    return n_lin > 0;
  }

  Bool Subtract::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }

  Bool Subtract::IsLinear(const Flags & linearMask, const Flags & knownMask) const
  {
    for (Size i = 0; i < linearMask.size(); ++i)
    {
      if (!(linearMask[i] || knownMask[i]))
        return false;
    }

    return linearMask[0] || linearMask[1];
  }

  Bool Multiply::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }

  // FIXME: valid with element wise operation ?
  Bool Multiply::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    Size n_factor = 0;
    for (Size i = 0; i < scaleMask.size(); ++i)
    {
      n_factor += scaleMask[i];
      if (!(scaleMask[i] || knownMask[i]))
        return false;
    }

    return n_factor == 1;
  }

  Bool Divide::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & denominator = paramValues[1];
    for (Size i = 0; i < denominator.Values().size(); ++i)
    {
      if (denominator.Values()[i] == 0.0) // FIXME use epsilon error comparison
        return false;
    }
    return true;
  }

  // FIXME: valid with element wise operation ?
  Bool Divide::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    return scaleMask[0] && knownMask[1];
  }

  Bool Neg::IsDiscreteValued(const Flags & mask) const
  {
    return mask[0];
  }

  Bool Neg::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    return scaleMask[0];
  }

}
