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

/*! \file DT.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DT.hpp"
#include <boost/math/special_functions/gamma.hpp>

namespace Biips
{

  static Scalar M_PI = 4 * std::atan(1);

  TDistType::TDistType(Scalar mu, Scalar tau, Scalar k) :
    mu_(mu), sqrtTau_(std::sqrt(tau)), k_(k), mathDist_(k), randomDist_(k)
  {
  }

  Scalar TDistType::pdf(Scalar t) const
  {
    Scalar y = (t - mu_) * sqrtTau_;
    return sqrtTau_ * boost::math::pdf(mathDist_, y);
  }

  Scalar TDistType::log_pdf(Scalar t) const
  {
    Scalar y = (t - mu_) * sqrtTau_;
    using std::log;
    using std::pow;
    using boost::math::lgamma;
    return lgamma(0.5 * (k_ + 1.0)) - lgamma(0.5 * k_) + log(sqrtTau_) - 0.5
        * log(k_ * M_PI) - 0.5 * (k_ + 1.0) * log(1.0 + pow(y, 2) / k_);
  }

  Scalar TDistType::cdf(Scalar t) const
  {
    Scalar y = (t - mu_) * sqrtTau_;
    return boost::math::cdf(mathDist_, y);

  }

  Scalar TDistType::quantile(Scalar p) const
  {
    return mu_ + boost::math::quantile(mathDist_, p) / sqrtTau_;
  }

  Bool DT::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar tau = paramValues[1].ScalarView();
    Scalar k = paramValues[2].ScalarView();
    return (tau > 0.0) && (k > 0.0);
  }

  DT::MathDistType DT::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar mu = paramValues[0].ScalarView();
    Scalar tau = paramValues[1].ScalarView();
    Scalar k = paramValues[2].ScalarView();
    return MathDistType(mu, tau, k);
  }

  DT::RandomDistType DT::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar mu = paramValues[0].ScalarView();
    Scalar tau = paramValues[1].ScalarView();
    Scalar k = paramValues[2].ScalarView();
    return RandomDistType(mu, tau, k);
  }

  Scalar DT::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    MathDistType dist = mathDist(paramValues);

    if (give_log)
      return boost::math::log_pdf(dist, x);
    else
      return boost::math::pdf(dist, x);
  }
}
