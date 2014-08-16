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

/*! \file DNormVar.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DNormVar.hpp"
#include "rng/TruncatedNormal.hpp"

namespace Biips
{

  Bool DNormVar::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar var = paramValues[1].ScalarView();
    return isFinite(mean) && var > 0.0;
  }

  DNormVar::MathDistType DNormVar::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar var = paramValues[1].ScalarView();

    using std::sqrt;
    return MathDistType(mean, sqrt(var));
  }

  DNormVar::RandomDistType DNormVar::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar var = paramValues[1].ScalarView();

    using std::sqrt;
    return RandomDistType(mean, sqrt(var));
  }

  Scalar DNormVar::d(Scalar x,
                     const NumArray::Array & paramValues,
                     Bool give_log) const
  {
    if (give_log)
    {
      Scalar mean = paramValues[0].ScalarView();
      Scalar var = paramValues[1].ScalarView();
      using std::log;
      using std::pow;
      return -0.5 * (LOG_2PI + log(var) + pow(x - mean, 2) / var);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }

  void DNormVar::sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar sigma = std::sqrt(paramValues[1].ScalarView());

    const NumArray & lower = boundValues.first;
    const NumArray & upper = boundValues.second;

    if (!lower.IsNULL() && !upper.IsNULL())
    {
      Scalar left = (lower.ScalarView() - mean) / sigma;
      Scalar right = (upper.ScalarView() - mean) / sigma;
      values[0] = mean + inormal(left, right, rng) * sigma;
    }
    else if (!lower.IsNULL())
    {
      Scalar left = (lower.ScalarView() - mean) / sigma;
      values[0] = mean + lnormal(left, rng) * sigma;
    }
    else if (!upper.IsNULL())
    {
      Scalar right = (upper.ScalarView() - mean) / sigma;
      values[0] = mean + rnormal(right, rng) * sigma;
    }
    else
      values[0] = r(paramValues, rng);
  }

}
