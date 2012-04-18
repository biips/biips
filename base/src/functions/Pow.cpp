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

/*! \file Pow.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "functions/Pow.hpp"

namespace Biips
{

  Bool Pow::checkParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & base = paramValues[0];
    const NumArray & exponent = paramValues[1];

    for (Size i = 0, j = 0; i < base.Values().size(); ++i)
    {
      if ((base.Values()[i] < 0.0) && !checkInteger(exponent.Values()[j]))
        return false;

      if ((base.Values()[i] == 0.0) && (exponent.Values()[j] < 0.0)) // FIXME use epsilon in equal comparison
        return false;

      if (!exponent.IsScalar())
        ++j;
    }
    return true;
  }

  Bool PowInfix::checkParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & base = paramValues[0];
    const NumArray & exponent = paramValues[1];

    for (Size i = 0, j = 0; i < base.Values().size(); ++i)
    {
      if ((base.Values()[i] < 0.0) && !checkInteger(exponent.Values()[j]))
        return false;

      if ((base.Values()[i] == 0.0) && (exponent.Values()[j] < 0.0)) // FIXME use epsilon in equal comparison
        return false;

      if (!exponent.IsScalar())
        ++j;
    }
    return true;
  }
}
