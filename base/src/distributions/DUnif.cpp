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

/*! \file DUnif.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DUnif.hpp"
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  Bool DUnif::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & lower = *paramDims[0];
    const DimArray & upper = *paramDims[1];
    return lower.IsScalar() && upper.IsScalar();
  }

  Bool DUnif::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar lower = paramValues[0].ScalarView();
    Scalar upper = paramValues[1].ScalarView();

    return lower < upper;
  }

  DimArray DUnif::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }

  void DUnif::sample(ValArray & values,
                     const NumArray::Array & paramValues,
                     const NumArray::Pair & boundValues,
                     Rng & rng) const
  {
    Scalar lower = paramValues[0].ScalarView();
    Scalar upper = paramValues[1].ScalarView();

    typedef boost::uniform_real<Scalar> DistType;
    DistType dist(lower, upper);
    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(rng.GetGen(), dist);

    values[0] = gen();
  }

  Scalar DUnif::logDensity(const NumArray & x,
                           const NumArray::Array & paramValues,
                           const NumArray::Pair & boundValues) const
  {
    Scalar lower = paramValues[0].ScalarView();
    Scalar upper = paramValues[1].ScalarView();
    Scalar my_point = x.ScalarView();

    if (my_point < lower || my_point > upper)
      return BIIPS_NEGINF;
    else
      return -std::log(upper - lower);
  }

  void DUnif::unboundedSupport(ValArray & lower,
                               ValArray & upper,
                               const NumArray::Array & paramValues) const
  {
    lower[0] = paramValues[0].ScalarView();
    upper[0] = paramValues[1].ScalarView();
  }

  Bool DUnif::IsSupportFixed(const Flags & fixmask) const
  {
    return fixmask[0] && fixmask[1]; //Lower and upper bounds fixed
  }
}
