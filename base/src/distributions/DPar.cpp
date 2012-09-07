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

/*! \file DPar.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DPar.hpp"

namespace Biips
{

  Bool DPar::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar c = paramValues[1].ScalarView();

    return (alpha >= 0.0) && (c <= 1.0);
  }

  DPar::MathDistType DPar::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar c = paramValues[1].ScalarView();

    return MathDistType(alpha, c);
  }

  DPar::RandomDistType DPar::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar c = paramValues[1].ScalarView();

    return RandomDistType(c, alpha);
  }

  Scalar DPar::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    Scalar c = paramValues[1].ScalarView();
    if (x < c)
      return give_log ? BIIPS_NEGINF : 0.0;

    if (give_log)
    {
      Scalar alpha = paramValues[0].ScalarView();
      using std::log;
      return log(alpha) + alpha * log(c) - (alpha + 1) * log(x);
    }

    MathDistType dist = mathDist(paramValues);

    return boost::math::pdf(dist, x);
  }

  Scalar DPar::unboundedLower(const NumArray::Array & paramValues) const
  {
    Scalar c = paramValues[1].ScalarView();
    return c;
  }

  Scalar DPar::unboundedUpper(const NumArray::Array & paramValues) const
  {
    return BIIPS_POSINF;
  }

  Bool DPar::IsSupportFixed(const Flags & fixmask) const
  {
    return fixmask[1]; //Fixed if C is fixed
  }

}
