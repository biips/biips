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

/*! \file DLnorm.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DLnorm.hpp"

namespace Biips
{

  using std::sqrt;
  using std::log;
  using std::pow;

  Bool DLnorm::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar tau = paramValues[1].ScalarView();
    return tau > 0.0;
  }

  DLnorm::MathDistType DLnorm::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar mu = paramValues[0].ScalarView();
    Scalar tau = paramValues[1].ScalarView();
    using std::sqrt;
    return MathDistType(mu, 1.0 / sqrt(tau));
  }

  DLnorm::RandomDistType DLnorm::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar mu = paramValues[0].ScalarView();
    Scalar tau = paramValues[1].ScalarView();
    using std::sqrt;
    return RandomDistType(mu, 1.0 / sqrt(tau));
  }

  Scalar DLnorm::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      Scalar mu = paramValues[0].ScalarView();
      Scalar tau = paramValues[1].ScalarView();
      using std::log;
      using std::pow;
      return 0.5 * log(tau) - log(x) - 0.5 * tau * pow(log(x) - mu, 2);
    }

    MathDistType dist = mathDist(paramValues);

    using namespace boost::math;
    return pdf(dist, x);
  }
}
