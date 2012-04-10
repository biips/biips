//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
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

/*! \file Rng.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_RNG_HPP_
#define BIIPS_RNG_HPP_

#include <boost/random/mersenne_twister.hpp>

namespace Biips
{
  
  class Rng
  {
  public:
    typedef Rng SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef boost::mt19937 GenType;
    typedef GenType::result_type ResultType;

  protected:
    GenType gen_;

  public:
    void Seed() { gen_.seed(); }
    void Seed(ResultType value) { gen_.seed(value); }
    GenType & GetGen() { return gen_; }

    Rng() {}
    explicit Rng(ResultType seed) : gen_(seed) {}
    virtual ~Rng() {}
  };

}

#endif /* BIIPS_RNG_HPP_ */
