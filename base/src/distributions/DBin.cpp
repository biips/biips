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

/*! \file DBin.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DBin.hpp"
#include <boost/math/special_functions/binomial.hpp>

namespace Biips
{

  Bool DBin::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar success_fraction = paramValues[0].ScalarView();
    Scalar trials = paramValues[1].ScalarView();

    return checkSize(trials) && (success_fraction >= 0.0) && (success_fraction
        <= 1.0);
  }

  DBin::MathDistType DBin::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar success_fraction = paramValues[0].ScalarView();
    Scalar trials = paramValues[1].ScalarView();

    return MathDistType(trials, success_fraction);
  }

  DBin::RandomDistType DBin::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar success_fraction = paramValues[0].ScalarView();
    Scalar trials = paramValues[1].ScalarView();

    return RandomDistType(trials, success_fraction);
  }

  Scalar DBin::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      Scalar success_fraction = paramValues[0].ScalarView();
      Scalar trials = paramValues[1].ScalarView();
      using std::log;
      using boost::math::binomial_coefficient;
      return log(binomial_coefficient<Scalar> (roundSize(trials), roundSize(x)))
          + x * log(success_fraction) + (trials - x) * log(1.0
          - success_fraction);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }

  Scalar DBin::unboundedLower(const NumArray::Array & paramValues) const
  {
    return 0.0;
  }

  Scalar DBin::unboundedUpper(const NumArray::Array & paramValues) const
  {
    const Scalar trials = paramValues[1].ScalarView();
    return trials;
  }

  Bool DBin::IsSupportFixed(const Flags & fixmask) const
  {
    return fixmask[1]; //trials number is fixed;
  }

}
