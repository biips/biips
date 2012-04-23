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

/*! \file DPois.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DPois.hpp"

#define LAMBDA(par) (par[0].ScalarView())

namespace Biips
{
  Bool DPois::checkParamValues(const NumArray::Array & paramValues) const
  {
    return LAMBDA(paramValues) >= 0.0;
  }

  Bool DPois::checkDensityParamValues(Scalar x,
                                      const NumArray::Array & paramValues) const
  {
    return checkSize(x) && checkParamValues(paramValues);
  }

  DPois::MathDistType DPois::mathDist(const NumArray::Array & paramValues) const
  {
    return MathDistType(LAMBDA(paramValues));
  }

  DPois::RandomDistType DPois::randomDist(const NumArray::Array & paramValues) const
  {
    return RandomDistType(LAMBDA(paramValues));
  }

  Scalar DPois::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      using std::log;
      Scalar log_d = -LAMBDA(paramValues) + x * log(LAMBDA(paramValues));
      for (Scalar i = 2.0; i - 0.5 < x; i += 1.0)
        log_d -= log(i);
      return log_d;
    }

    MathDistType dist = mathDist(paramValues);

    return boost::math::pdf(dist, x);
  }

}
