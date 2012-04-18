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

/*! \file DExp.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DExp.hpp"

namespace Biips
{

  Bool DExp::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar lambda = paramValues[0].ScalarView();
    return lambda > 0.0;
  }

  DExp::MathDistType DExp::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar lambda = paramValues[0].ScalarView();

    return MathDistType(lambda);
  }

  DExp::RandomDistType DExp::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar lambda = paramValues[0].ScalarView();

    return RandomDistType(lambda);
  }

  Scalar DExp::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      Scalar lambda = paramValues[0].ScalarView();
      return std::log(lambda) - lambda * x;
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }
}
