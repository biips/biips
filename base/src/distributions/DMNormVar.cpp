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

/*! \file DMNormVar.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#define _USE_MATH_DEFINES

#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include "distributions/DMNormVar.hpp"
#include "common/cholesky.hpp"

namespace Biips
{

  static const Scalar TOL = 1e-7;
  static const Scalar LOG_2PI = std::log(2 * M_PI);

  Bool DMNormVar::checkParamDims(
      const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & mean_dim = *paramDims[0];
    const DimArray & var_dim = *paramDims[1];
    if (!mean_dim.Drop().IsVector())
      return false;
    if (var_dim.IsSquared())
      return (mean_dim[0] == var_dim[0]);
    else
      return mean_dim.IsScalar() && var_dim.IsScalar();
  }

  DimArray DMNormVar::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return paramDims[0]->Drop();
  }

  Bool DMNormVar::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & mean = paramValues[0];
    const NumArray & var = paramValues[1];


    Size n = mean.Length();

    // FIXME: this is only valid for Column major order
    for (Size i = 0; i < n; ++i)
    {
      if (!isFinite(mean.Values()[i]))
        return false;
      Size x_ind = i;
      Size y_ind = n * i;
      for (Size j = 0; j < i; ++j)
      {
        if (std::fabs(var.Values()[x_ind] - var.Values()[y_ind]) > TOL)
          return false;
        x_ind += n;
        y_ind++;
      }
    }

    return true;
  }

  void DMNormVar::sample(ValArray & values, const NumArray::Array & paramValues,
                         const NumArray::Pair & boundValues, Rng & rng) const
  {
    const NumArray & mean = paramValues[0];
    const NumArray & var = paramValues[1];

    Size n_dim = mean.Values().size();

    Matrix var_chol(var);
    if (!ublas::cholesky_factorize(var_chol))
      throw RuntimeError(
          "DMNormVar::sample: matrix is not positive-semidefinite.");

    typedef boost::normal_distribution<Scalar> DistType;
    boost::variate_generator<Rng::GenType&, DistType> gen(rng.GetGen(),
                                                          DistType());

    std::generate(values.begin(), values.end(), gen);

    ublas::vector<Scalar, ValArray> sample_vec(values.size(), ValArray());
    sample_vec.data().swap(values);
    sample_vec = ublas::prod(
        ublas::triangular_adaptor<Matrix, ublas::lower>(var_chol), sample_vec);
    values.swap(sample_vec.data());

    for (Size i = 0; i < n_dim; ++i)
      values[i] += mean.Values()[i];

  }

  Scalar DMNormVar::logDensity(const NumArray & x,
                               const NumArray::Array & paramValues,
                               const NumArray::Pair & boundValues) const
  {
    const NumArray & mean = paramValues[0];
    const NumArray & var = paramValues[1];

    Vector diff_vec(x.Length(), x.Values() - mean.Values());

    Matrix var_chol(var);
    if (!ublas::cholesky_factorize(var_chol))
      throw RuntimeError(
          "DMNormVar::logDensity: matrix is not positive-semidefinite.");

    ublas::inplace_solve(var_chol, diff_vec, ublas::lower_tag());

    return -0.5
           * (diff_vec.size() * LOG_2PI + ublas::cholesky_logdet(var_chol)
              + ublas::inner_prod(diff_vec, diff_vec));
  }

  void DMNormVar::fixedUnboundedSupport(ValArray & lower, ValArray & upper,
                                   const NumArray::Array & paramValues) const
  {
    std::fill(lower.begin(), lower.end(), BIIPS_NEGINF);
    std::fill(upper.begin(), upper.end(), BIIPS_POSINF);
  }

}
