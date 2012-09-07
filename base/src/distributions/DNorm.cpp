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

/*! \file DNorm.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DNorm.hpp"
#include "rng/TruncatedNormal.hpp"

namespace Biips
{

  Bool DNorm::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar prec = paramValues[1].ScalarView();
    return isFinite(mean) && prec > 0.0;
  }

  DNorm::MathDistType DNorm::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar prec = paramValues[1].ScalarView();

    using std::sqrt;
    return MathDistType(mean, 1.0 / sqrt(prec));
  }

  DNorm::RandomDistType DNorm::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar prec = paramValues[1].ScalarView();

    using std::sqrt;
    return RandomDistType(mean, 1.0 / sqrt(prec));
  }

  Scalar DNorm::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      Scalar mean = paramValues[0].ScalarView();
      Scalar prec = paramValues[1].ScalarView();
      using std::log;
      using std::pow;
      return -0.5 * (log(2 * M_PI) - log(prec) + pow(x - mean, 2) * prec);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }

  void DNorm::sample(ValArray & values,
                     const NumArray::Array & paramValues,
                     const NumArray::Pair & boundValues,
                     Rng & rng) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar sigma = 1.0 / std::sqrt(paramValues[1].ScalarView());

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
