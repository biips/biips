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

namespace Biips
{

  Bool DBeta::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar beta = paramValues[1].ScalarView();

    return alpha > 0.0 && beta > 0.0;
  }

  Bool DBeta::checkDensityParamValues(Scalar x,
                                      const NumArray::Array & paramValues) const
  {
    if (checkParamValues(paramValues))
    {
      Scalar alpha = paramValues[0].ScalarView();
      Scalar beta = paramValues[1].ScalarView();
      return !(x == 0 && alpha < 1) && !(x == 1 && beta < 1);
    }
    else
      return false;
  }

  DBeta::MathDistType DBeta::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar beta = paramValues[1].ScalarView();

    return MathDistType(alpha, beta);
  }

  DBeta::RandomDistType DBeta::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar beta = paramValues[1].ScalarView();

    return RandomDistType(alpha, beta);
  }

  Scalar DBeta::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      Scalar alpha = paramValues[0].ScalarView();
      Scalar beta = paramValues[1].ScalarView();

      using std::log;
      using boost::math::lgamma;

      return lgamma(alpha + beta) - lgamma(alpha) - lgamma(beta)
          + (alpha - 1.0) * log(x) + (beta - 1.0) * log(1.0 - x);
    }

    MathDistType dist = mathDist(paramValues);

    return boost::math::pdf(dist, x);
  }
}
