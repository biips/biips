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

/*! \file TruncatedNormal.cpp
* \brief
*
* \author  $LastChangedBy$
* \date    $LastChangedDate$
* \version $LastChangedRevision$
* Id:      $Id$
*
* COPY: Adapted from JAGS TruncatedNormal.cc
*/

#include "rng/TruncatedNormal.hpp"
#include "rng/Rng.hpp"
#include "common/Error.hpp"
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/variate_generator.hpp>

using std::sqrt;
using std::exp;


namespace Biips
{

  //sqrt(2*pi)
  static const Scalar STP = 2.506628274631;

  typedef boost::normal_distribution<Scalar> NormDistType;
  typedef boost::uniform_01<Scalar> UnifDistType;
  typedef boost::exponential_distribution<Scalar> ExpDistType;

  static NormDistType norm_dist;
  static UnifDistType unif_dist;
  static ExpDistType exp_dist;

  typedef boost::variate_generator<Rng::GenType&, NormDistType> NormGenType;
  typedef boost::variate_generator<Rng::GenType&, UnifDistType> UnifGenType;
  typedef boost::variate_generator<Rng::GenType&, ExpDistType> ExpGenType;

  static inline NormGenType normGen(Rng & rng)
  {
    return NormGenType(rng.GetGen(), norm_dist);
  }

  static inline UnifGenType unifGen(Rng & rng)
  {
    return UnifGenType(rng.GetGen(), unif_dist);
  }

  static inline ExpGenType expGen(Rng & rng)
  {
    return ExpGenType(rng.GetGen(), exp_dist);
  }

  //Calculates optimal scale parameter for exponential envelope
  static Scalar Alpha(Scalar mu)
  {
    return (mu + sqrt(mu*mu + 4.0))/2.0;
  }

  static Scalar lnorm(Scalar left, Rng & rng)
  {
    if (!isFinite(left))
      throw LogicError("Non-finite boundary in truncated normal");

    NormGenType norm_gen = normGen(rng);
    if (left < 0)
    {
      //Repeated sampling until truncation satisfied
      while(true)
      {
        Scalar y = norm_gen();
        if (y >= left)
          return y;
      }
    }
    else
    {
      //Rejection sampling with exponential envelope
      Scalar alpha = Alpha(left);
      ExpGenType exp_gen = expGen(rng);
      UnifGenType unif_gen = unifGen(rng);
      while (true)
      {
        Scalar y = exp_gen();
        Scalar C = (1 - y)/alpha;
        if (unif_gen() <= exp(-C*C/2))
          return left + y/alpha;
        /*
              //Original version, less stable for large mu
              Scalar z = left + exp_gen() / alpha;
              if (unif_gen() <= exp(-(alpha - z)*(alpha - z)/2))
                  return z;
         */
      }
    }
  }

  static Scalar rnorm(Scalar right, Rng & rng)
  {
    return -lnorm(-right, rng);
  }

  /*
     Rejection sampling of interval-truncated standard normal
     distribution using a uniform envelope.  This works best when we are
     sampling a short interval.x
   */
  static Scalar inorm_unif(Scalar left, Scalar right, Rng & rng)
  {
    Scalar zmax; //Value of z at which density is maximum
    if (left > 0)
      zmax = left;
    else if (right < 0)
      zmax = right;
    else
      zmax = 0;

    UnifGenType unif_gen = unifGen(rng);
    while (true)
    {
      Scalar z = left + (right - left) * unif_gen();
      if (unif_gen() <= exp((zmax*zmax - z*z)/2))
        return z;
    }
  }

  /*
    Sampling from a interval-truncated standard normal by repeated draws
    from a non-truncated standard normal distribution. We keep sampling
    until the sampled value lies in the required interval.  This works
    best when we are sampling a wide interval, and are not sampling from
    one of the tails (so that left < 0, right > 0).
   */
  static Scalar inorm_repeat(Scalar left, Scalar right, Rng & rng)
  {
    NormGenType norm_gen = normGen(rng);
    Scalar y = norm_gen();
    while (y < left || y > right)
      y = norm_gen();

    return y;
  }

  /*
    Sample an interval-truncated normal distribution when the
    interval [left,right] is in the right tail of the underlying normal
    distribution (so left > 0).

    If the interval is short, we do this by rejection
    sampling. Otherwise we do it by repeated sampling from a left
    truncated normal until the right limit is also satisfied.
   */
  static Scalar inorm_right_tail(Scalar left, Scalar right, Rng & rng)
  {
    Scalar alpha = Alpha(left);
    Scalar maxdiff = exp((left*left - alpha*left + 1)/2)/alpha;
    if (right - left <= maxdiff)
    {
      //Rejection sampling with uniform envelope
      return inorm_unif(left, right, rng);
    }
    else
    {
      //Interval too wide for rejection sampling. Do repeat sampling
      //from left-truncated normal
      Scalar y = lnorm(left, rng);
      while (y >= right)
        y = lnorm(left, rng);
      return y;
    }
  }


  static Scalar inorm(Scalar left, Scalar right, Rng & rng)
  {
    if (!isFinite(left) || !isFinite(right))
      throw LogicError("Non-finite boundary in truncated normal");

    if (right < left)
      throw LogicError("Invalid limits in inorm");

    if (left > 0)
      return inorm_right_tail(left, right, rng);

    else if (right < 0)
      return -inorm_right_tail(-right, -left, rng);

    else if (right - left < STP)
      return inorm_unif(left, right, rng);

    else
      return inorm_repeat(left, right, rng);
  }

  /* Public interface */

  Scalar lnormal(Scalar left, Rng & rng, Scalar mu, Scalar sigma)
  {
    return mu + sigma * lnorm((left - mu)/sigma, rng);
  }

  Scalar rnormal(Scalar right, Rng & rng, Scalar mu, Scalar sigma)
  {
    return mu + sigma * rnorm((right - mu)/sigma, rng);
  }

  Scalar inormal(Scalar left, Scalar right, Rng & rng, Scalar mu, Scalar sigma)
  {
    return mu + sigma * inorm((left - mu)/sigma, (right - mu)/sigma, rng);
  }

}
