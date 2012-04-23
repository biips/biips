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

/*! \file DBeta.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DBETA_HPP_
#define BIIPS_DBETA_HPP_

#include "distributions/BoostScalarDistribution.hpp"

#include <boost/random/beta_distribution.hpp>
#include <boost/math/distributions/beta.hpp>

namespace Biips
{
  typedef boost::math::beta_distribution<Scalar> BetaMathDistType;
  typedef boost::beta_distribution<Scalar> BetaRandomDistType;

  class DBeta: public BoostScalarDistribution<BetaMathDistType,
      BetaRandomDistType>
  {
  public:
    typedef DBeta SelfType;
    typedef BoostScalarDistribution<BetaMathDistType, BetaRandomDistType>
        BaseType;

  protected:
    DBeta() :
      BaseType("dbeta", 2, DIST_PROPORTION, false)
    {
    }
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;
    virtual Bool
    checkDensityParamValues(Scalar x, const NumArray::Array & paramValues) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    Scalar d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const;

    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
    ;
  };

}

#endif /* BIIPS_DBETA_HPP_ */
