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

/*! \file DGamma.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DGamma.hpp"
#include <boost/math/special_functions/gamma.hpp>

namespace Biips
{

  Bool DGamma::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar shape = paramValues[0].ScalarView();
    Scalar inv_scale = paramValues[1].ScalarView();
    return (shape > 0.0) && (inv_scale > 0.0);
  }

  DGamma::MathDistType DGamma::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar shape = paramValues[0].ScalarView();
    Scalar inv_scale = paramValues[1].ScalarView();

    return MathDistType(shape, 1.0 / inv_scale);
  }

  DGamma::RandomDistType DGamma::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar shape = paramValues[0].ScalarView();
    Scalar inv_scale = paramValues[1].ScalarView();

    return RandomDistType(shape, 1.0 / inv_scale);
  }

  Scalar DGamma::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (x < 0.0)
      return give_log ? BIIPS_NEGINF : 0.0;

    if (give_log)
    {
      Scalar shape = paramValues[0].ScalarView();
      Scalar inv_scale = paramValues[1].ScalarView();
      using std::log;
      using boost::math::lgamma;
      return shape * log(inv_scale) + (shape - 1.0) * log(x) - inv_scale * x
          - lgamma(shape);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }
}
