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

/*! \file DPar.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DPAR_HPP_
#define BIIPS_DPAR_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/pareto_distribution.hpp>
#include <boost/math/distributions/pareto.hpp>

namespace Biips
{

  typedef boost::math::pareto_distribution<Scalar> ParMathDistType;
  typedef boost::pareto_distribution<Scalar> ParRandomDistType;

  class DPar: public BoostScalarDistribution<ParMathDistType, ParRandomDistType>
  {
  public:
    typedef DPar SelfType;
    typedef BoostScalarDistribution<ParMathDistType, ParRandomDistType>
        BaseType;

  protected:
    DPar() :
      BaseType("dpar", 2, DIST_SPECIAL, false)
    {
    }
    virtual Scalar fixedUnboundedLower(const NumArray::Array & paramValues) const;
    virtual Scalar fixedUnboundedUpper(const NumArray::Array & paramValues) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "dbinom";
    }
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;
    virtual Bool IsSupportFixed(const Flags & fixmask) const;
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

#endif /* BIIPS_DPAR_HPP_ */
