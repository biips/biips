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

/*! \file DBern.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DBern.hpp"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  Bool DBern::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & p = *paramDims[0];
    return p.IsScalar();
  }

  Bool DBern::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar p = paramValues[0].ScalarView();

    return (p > 0.0) && (p < 1.0);
  }

  DimArray DBern::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }

  void DBern::sample(ValArray & values,
                     const NumArray::Array & paramValues,
                     const NumArray::Pair & boundValues,
                     Rng & rng) const
  {
    Scalar p = paramValues[0].ScalarView();

    typedef boost::bernoulli_distribution<Scalar> DistType;
    DistType dist(p);
    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(rng.GetGen(), dist);

    values[0] = gen();
  }

  Scalar DBern::logDensity(const NumArray & x,
                           const NumArray::Array & paramValues,
                           const NumArray::Pair & boundValues) const
  {
    Scalar p = paramValues[0].ScalarView();
    Scalar my_point = x.ScalarView();

    using std::log;
    return my_point * log(p) + (1.0 - my_point) * log(1.0 - p);
  }

  void DBern::unboundedSupport(ValArray & lower,
                               ValArray & upper,
                               const NumArray::Array & paramValues) const
  {
    lower[0] = 0.0;
    upper[0] = 1.0;
  }

}
