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

/*! \file DBin.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DBIN_HPP_
#define BIIPS_DBIN_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/binomial_distribution.hpp>
#include <boost/math/distributions/binomial.hpp>

namespace Biips
{

  typedef boost::math::binomial_distribution<Scalar> BinMathDistType;
  typedef boost::binomial_distribution<Scalar> BinRandomDistType;

  class DBin: public BoostScalarDistribution<BinMathDistType, BinRandomDistType>
  {
  public:
    typedef DBin SelfType;
    typedef BoostScalarDistribution<BinMathDistType, BinRandomDistType>
        BaseType;

  protected:
    DBin() :
      BaseType("dbin", 2, DIST_SPECIAL, true)
    {
    }
    virtual Scalar unboundedLower(const NumArray::Array & paramValues) const;
    virtual Scalar unboundedUpper(const NumArray::Array & paramValues) const;

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

#endif /* BIIPS_DBIN_HPP_ */
