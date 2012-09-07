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

/*! \file DBeta.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DBeta.hpp"
#include <boost/math/special_functions/gamma.hpp>

#define ALPHA(paramValues) (paramValues[0].ScalarView())
#define BETA(paramValues) (paramValues[1].ScalarView())

namespace Biips
{

  Bool DBeta::CheckParamValues(const NumArray::Array & paramValues) const
  {
    return ALPHA(paramValues) > 0.0 && BETA(paramValues) > 0.0;
  }

  Bool DBeta::checkDensityParamValues(Scalar x,
                                      const NumArray::Array & paramValues) const
  {
    if (ALPHA(paramValues) > 0.0 && BETA(paramValues) > 0.0)
    {
      if (x == 0 && ALPHA(paramValues) < 1)
        return false;
      else if (x == 1 && BETA(paramValues) < 1)
        return false;
      return true;
    }
    return false;
  }

  DBeta::MathDistType DBeta::mathDist(const NumArray::Array & paramValues) const
  {
    return MathDistType(ALPHA(paramValues), BETA(paramValues));
  }

  DBeta::RandomDistType DBeta::randomDist(
      const NumArray::Array & paramValues) const
  {
    return RandomDistType(ALPHA(paramValues), BETA(paramValues));
  }

  Scalar DBeta::d(Scalar x, const NumArray::Array & paramValues,
                  Bool give_log) const
  {
    if (give_log)
    {
      Scalar alpha = ALPHA(paramValues);
      Scalar beta = BETA(paramValues);

      using std::log;
      using boost::math::lgamma;

      return lgamma(alpha + beta) - lgamma(alpha) - lgamma(beta)
             + (alpha - 1.0) * log(x) + (beta - 1.0) * log(1.0 - x);
    }

    MathDistType dist = mathDist(paramValues);

    return boost::math::pdf(dist, x);
  }

  Scalar DBeta::r(const NumArray::Array & paramValues, Rng & rng) const
  {
    typedef boost::variate_generator<Rng::GenType&, RandomDistType> GenType;
    GenType gen(rng.GetGen(), randomDist(paramValues));

    Scalar ans = 0;
    // discard 0 and 1 values
    // because pdf not defined
    while (!(ans > 0 && ans < 1))
      ans = gen();
    return ans;
  }
}
