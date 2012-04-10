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

/*! \file TrigonometricFunctions.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "functions/TrigonometricFunctions.hpp"

namespace Biips
{

  Bool ArcCos::checkParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] < -1.0 || x[i] > 1.0)
        return false;
    }
    return true;
  }

  Bool ArcSin::checkParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] < -1.0 || x[i] > 1.0)
        return false;
    }
    return true;
  }

  Bool ArcCosh::checkParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] < 1.0)
        return false;
    }
    return true;
  }

  Bool ArcTanh::checkParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] <= -1.0 || x[i] >= 1.0)
        return false;
    }
    return true;
  }
}
