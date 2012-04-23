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

/*! \file DWeib.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DWeib.hpp"

namespace Biips
{

  Bool DWeib::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar v = paramValues[0].ScalarView();
    Scalar lambda = paramValues[1].ScalarView();
    return (v > 0.0) && (lambda > 0.0);
  }

  DWeib::MathDistType DWeib::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar shape = paramValues[0].ScalarView();
    using std::pow;
    Scalar scale = pow(paramValues[1].ScalarView(), -1 / shape);

    return MathDistType(shape, scale);
  }

  DWeib::RandomDistType DWeib::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar shape = paramValues[0].ScalarView();
    using std::pow;
    Scalar scale = pow(paramValues[1].ScalarView(), -1 / shape);

    return RandomDistType(scale, shape);
  }

  Scalar DWeib::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (x == 0.0)
      return give_log ? BIIPS_NEGINF : 0.0;

    if (give_log)
    {
      Scalar v = paramValues[0].ScalarView();
      Scalar lambda = paramValues[1].ScalarView();
      using std::log;
      using std::pow;
      return log(v) + log(lambda) + (v - 1) * log(x) - lambda * pow(x, v);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }
}
