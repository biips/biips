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

/*! \file Particle.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from SMCTC smc::particle class
 */

#ifndef BIIPS_PARTICLE_HPP_
#define BIIPS_PARTICLE_HPP_

#include "graph/GraphTypes.hpp"

namespace Biips
{

  class Particle
  {
  private:
    typedef NodeValues Space;

    Space value_;
    Scalar logWeight_;
    Scalar weight_;

  public:
    Particle() : logWeight_(std::numeric_limits<Scalar>::quiet_NaN()), weight_(std::numeric_limits<Scalar>::quiet_NaN()) {};
    Particle(Space init, Scalar logWeight) : value_(init), logWeight_(logWeight), weight_(std::exp(logWeight)) {};

    const Space & GetValue() const { return value_; }
    Space & Value() { return value_; }

    Scalar LogWeight() const { return logWeight_; }
    Scalar Weight() const { return weight_; }

    void ResetWeight() { logWeight_ = 0.0; weight_ = 1.0; }
    void AddToLogWeight(Scalar increment);
  };

}

#endif /* BIIPS_PARTICLE_HPP_ */
