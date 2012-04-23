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

/*! \file DDexp.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DDexp.hpp"

namespace Biips
{
  static inline Scalar MU(const NumArray::Array & par)
  {
    return par[0].ScalarView();
  }

  static inline Scalar RATE(const NumArray::Array & par)
  {
    return par[1].ScalarView();
  }

  DexpDistType::DexpDistType(Scalar mu, Scalar rate) :
    location_(mu), scale_(1.0 / rate), mathDist_(scale_),
        expRandomDist_(scale_)
  {
  }

  Bool DDexp::checkParamValues(const NumArray::Array & paramValues) const
  {
    return RATE(paramValues) > 0.0;
  }

  DDexp::MathDistType DDexp::mathDist(const NumArray::Array & paramValues) const
  {
    return MathDistType(MU(paramValues), RATE(paramValues));
  }

  DDexp::RandomDistType DDexp::randomDist(const NumArray::Array & paramValues) const
  {
    return RandomDistType(MU(paramValues), RATE(paramValues));
  }

  Scalar DDexp::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
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
