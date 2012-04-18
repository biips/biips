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

/*! \file TruncatedNormal.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from JAGS TruncatedNormal.h
 */

#ifndef BIIPS_TRUNCATEDNORMAL_HPP_
#define BIIPS_TRUNCATEDNORMAL_HPP_

#include "common/Types.hpp"

namespace Biips
{

  class Rng;

  /**
   * Draws a random sample from a left-truncated normal distribution.
   *
   * @param left Left limit of the truncated distribution
   * @param rng Pointer to a Random Number Generator
   * @param mu Mean of untruncated distribution
   * @param sigma Standard deviation of untruncated distribution
   */
  Scalar lnormal(Scalar left, Rng & rng, Scalar mu = 0.0, Scalar sigma = 1.0);

  /**
   * Draws a random sample from a right-truncated normal distribution.
   *
   * @param right Right limit of the distribution
   * @param rng Pointer to a Random Number Generator
   * @param mu Mean of untruncated distribution
   * @param sigma Standard deviation of untruncated distribution
   */
  Scalar rnormal(Scalar right, Rng & rng, Scalar mu = 0.0, Scalar sigma = 1.0);

  /**
   * Draws a random sample from an interval-truncated normal distribution.
   *
   * @param left Left limit of the distribution
   * @param right Right limit of the distribution
   * @param rng Pointer to a Random Number Generator
   * @param mu Mean of untruncated distribution
   * @param sigma Standard deviation of untruncated distribution
   */
  Scalar inormal(Scalar left, Scalar right, Rng & rng,
                 Scalar mu = 0.0, Scalar sigma = 1.0);
}

#endif /* BIIPS_TRUNCATEDNORMAL_HPP_ */
