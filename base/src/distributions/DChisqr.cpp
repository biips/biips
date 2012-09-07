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

/*! \file DChisqr.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DChisqr.hpp"
#include <boost/math/special_functions/gamma.hpp>

namespace Biips
{

  Bool DChisqr::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar degree = paramValues[0].ScalarView();
    return degree > 0.0;
  }

  DChisqr::MathDistType DChisqr::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar degree = paramValues[0].ScalarView();

    return MathDistType(degree);
  }

  DChisqr::RandomDistType DChisqr::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar degree = paramValues[0].ScalarView();

    return RandomDistType(degree);
  }

  Scalar DChisqr::d(Scalar x,
                    const NumArray::Array & paramValues,
                    Bool give_log) const
  {
    if (x < 0.0)
      return give_log ? BIIPS_NEGINF : 0.0;

    if (give_log)
    {
      Scalar degree = paramValues[0].ScalarView();
      using std::log;
      using boost::math::lgamma;
      return 0.5 * ((degree - 0.5) * log(x) - x - degree * log(2.0))
          - lgamma(0.5 * degree);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }

}
