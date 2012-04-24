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

/*! \file DF.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DF.hpp"
#include <boost/math/special_functions/gamma.hpp>

namespace Biips
{

  Bool DF::checkParamValues(const NumArray::Array & paramValues) const
  {
    return (paramValues[0].ScalarView() > 0.0) && (paramValues[1].ScalarView()
        > 0.0);
  }

  Bool DF::checkDensityParamValues(Scalar x,
                                   const NumArray::Array & paramValues) const
  {
    return x > 0.0 && checkParamValues(paramValues);
  }

  DF::MathDistType DF::mathDist(const NumArray::Array & paramValues) const
  {
    return MathDistType(paramValues[0].ScalarView(),
                        paramValues[1].ScalarView());
  }

  DF::RandomDistType DF::randomDist(const NumArray::Array & paramValues) const
  {
    return RandomDistType(paramValues[0].ScalarView(),
                          paramValues[1].ScalarView());
  }

  Scalar DF::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      Scalar n1 = paramValues[0].ScalarView();
      Scalar n2 = paramValues[1].ScalarView();

      Scalar s = 0.5 * (n1 + n2);
      Scalar f = n1 / n2;

      using std::log;
      using boost::math::lgamma;

      return lgamma(s) - lgamma(0.5 * n1) - lgamma(0.5 * n2) + 0.5 * n1
          * log(f) + (0.5 * n1 - 1.0) * log(x) - s * log(1.0 + f * x);
    }

    MathDistType dist = mathDist(paramValues);

    return boost::math::pdf(dist, x);
  }

}
