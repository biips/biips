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

/*! \file Resampler.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_RESAMPLER_HPP_
#define BIIPS_RESAMPLER_HPP_

#include "Particle.hpp"
#include "common/Table.hpp"

namespace Biips
{

  class Resampler
  {
  public:
    typedef Resampler SelfType;
    typedef Types<Resampler>::Ptr Ptr;

  protected:
    const String name_;
    Size nParticles_;
    ValArray weights_;
    Types<Size>::Array count_;
    Types<Size>::Array indices_;

    virtual void resample(Scalar sumOfWeights, Rng & rng) = 0;

  public:
    Resampler(const String & name) :
      name_(name), nParticles_(0)
    {
    }

    String const & Name() const
    {
      return name_;
    }
    virtual String Alias() const
    {
      return "";
    }

    void Resample(Types<Particle>::Array & particles,
                  Scalar & sumOfWeights,
                  Rng & rng);

    virtual ~Resampler()
    {
    }
  };

  class ResamplerTable: public Table<Resampler>
  {
  public:
    ResamplerTable();
  };
}

#endif /* BIIPS_RESAMPLER_HPP_ */
