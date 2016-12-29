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

/*! \file   DInterval.cpp
 * \brief   
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */
#include <distributions/DInterval.hpp>

namespace Biips
{

  static Size value(Scalar t, const NumArray & cutpoints)
  {
    for (Size i = 0; i < cutpoints.Length(); ++i) {
      if (t <= cutpoints.Values()[i])
        return i;
    }
    return cutpoints.Length();
  }

  Bool DInterval::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return paramDims[0]->IsScalar();
  }

  Bool DInterval::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & cutpoints = paramValues[1];

    for (Size i=1; i<cutpoints.Length(); ++i)
    {
      if (cutpoints.Values()[i] <= cutpoints.Values()[i-1])
        return false;
    }
    return true;
  }

  DimArray DInterval::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }

  void DInterval::sample(ValArray & values,
                    const NumArray::Array & paramValues,
                    const NumArray::Pair & boundValues,
                    Rng & rng) const
  {
    /* sample for Dinterval is deterministic */
    const Scalar t = paramValues[0].ScalarView();
    const NumArray & cutpoints = paramValues[1];

    values[0] = static_cast<Scalar>(value(t, cutpoints));
  }

  Scalar DInterval::logDensity(const NumArray & x,
                          const NumArray::Array & paramValues,
                          const NumArray::Pair & boundValues) const
  {
    if (x.ScalarView() < 0)
      return BIIPS_NEGINF;

    Size x_int = static_cast<Size>(x.ScalarView());
    const NumArray & cutpoints = paramValues[1];

    if (x_int > cutpoints.Length()) {
      return BIIPS_NEGINF;
    }
    else {
      const Scalar t = paramValues[0].ScalarView();
      if (x_int > 0 && t <= cutpoints.Values()[x_int-1])
        return BIIPS_NEGINF;
      else if (x_int < cutpoints.Length() && t > cutpoints.Values()[x_int])
        return BIIPS_NEGINF;
      else
        return 0;
    }

  }

  void DInterval::fixedUnboundedSupport(ValArray & lower,
                              ValArray & upper,
                              const NumArray::Array & paramValues) const
  {
    const Scalar t = paramValues[0].ScalarView();
    const NumArray & cutpoints = paramValues[1];

    Size y = value(t, cutpoints);
    lower[0] = static_cast<Scalar>(y);
    upper[0] = static_cast<Scalar>(y);
  }

} /* namespace Biips */
