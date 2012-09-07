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

/*! \file DChisqr.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DCHISQR_HPP_
#define BIIPS_DCHISQR_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/chi_squared_distribution.hpp>
#include <boost/math/distributions/chi_squared.hpp>

namespace Biips
{

  typedef boost::math::chi_squared_distribution<Scalar> ChisqrMathDistType;
  typedef boost::chi_squared_distribution<Scalar> ChisqrRandomDistType;

  class DChisqr: public BoostScalarDistribution<ChisqrMathDistType,
      ChisqrRandomDistType>
  {
  public:
    typedef DChisqr SelfType;
    typedef BoostScalarDistribution<ChisqrMathDistType, ChisqrRandomDistType>
        BaseType;

  protected:
    DChisqr() :
      BaseType("dchisqr", 1, DIST_POSITIVE, false)
    {
    }

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "dchisq";
    }
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;
    virtual Scalar d(Scalar x,
                     const NumArray::Array & paramValues,
                     Bool give_log) const;
    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_DCHISQR_HPP_ */
