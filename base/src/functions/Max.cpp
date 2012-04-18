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

/*! \file Max.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "functions/Max.hpp"

namespace Biips
{

  void Max::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    Scalar ans = *std::max_element(paramValues[0].Values().begin(),
                                  paramValues[0].Values().end());
    for (Size i = 1; i < paramValues.size(); ++i)
    {
      Scalar max_i = *std::max_element(paramValues[i].Values().begin(),
                                      paramValues[i].Values().end());
      ans = std::max(ans, max_i);
    }
    values[0] = ans;
  }

  Bool Max::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }
}

