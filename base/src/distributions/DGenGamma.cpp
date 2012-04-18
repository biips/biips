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

/*! \file DGenGamma.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DGenGamma.hpp"
#include <boost/math/special_functions/gamma.hpp>

namespace Biips
{
  static inline Scalar SHAPE(const NumArray::Array & par)
  {
    return par[0].ScalarView();
  }

  static inline Scalar RATE(const NumArray::Array & par)
  {
    return par[1].ScalarView();
  }

  static inline Scalar POW(const NumArray::Array & par)
  {
    return par[2].ScalarView();
  }

  Scalar GenGammaDistType::pdf(Scalar t) const
  {
    if (t < 0.0)
      return 0.0;

    using std::log;
    using std::exp;
    using boost::math::pdf;

    Scalar log_jacobian = (pow_ - 1) * log(t) + log(pow_) - pow_ * log(scale_);
    return exp(log_jacobian) * pdf(mathDist_, transform(t));
  }

  Scalar GenGammaDistType::log_pdf(Scalar t) const
  {
    if (t < 0.0)
      return BIIPS_NEGINF;

    using std::log;
    using boost::math::lgamma;

    Scalar log_jacobian = (pow_ - 1) * log(t) + log(pow_) - pow_ * log(scale_);

    t = transform(t);

    return log_jacobian - shape_ * log(scale_) + (shape_ - 1.0) * log(t) - t
        / scale_ - lgamma(shape_);
  }

  GenGammaDistType::GenGammaDistType(Scalar shape, Scalar rate, Scalar pow) :
    shape_(shape), scale_(1.0 / rate), pow_(pow), mathDist_(shape, 1.0),
        randomDist_(shape, 1.0)
  {
  }

  Bool DGenGamma::checkParamValues(const NumArray::Array & paramValues) const
  {
    return SHAPE(paramValues) > 0.0 && RATE(paramValues) > 0.0
        && POW(paramValues) > 0.0;
  }

  DGenGamma::MathDistType DGenGamma::mathDist(const NumArray::Array & paramValues) const
  {
    return MathDistType(SHAPE(paramValues), RATE(paramValues), POW(paramValues));
  }

  DGenGamma::RandomDistType DGenGamma::randomDist(const NumArray::Array & paramValues) const
  {
    return RandomDistType(SHAPE(paramValues),
                          RATE(paramValues),
                          POW(paramValues));
  }

  Scalar DGenGamma::d(Scalar x,
                      const NumArray::Array & paramValues,
                      Bool give_log) const
  {
    MathDistType dist = mathDist(paramValues);

    if (give_log)
    {
      return boost::math::log_pdf(dist, x);
    }
    else
      return boost::math::pdf(dist, x);
  }
}
