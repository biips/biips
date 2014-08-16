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

/*! \file DWeib.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DWEIB_HPP_
#define BIIPS_DWEIB_HPP_

#include <boost/random/weibull_distribution.hpp>
#include <boost/math/distributions/weibull.hpp>
#include "distributions/BoostScalarDistribution.hpp"

namespace Biips
{

  typedef boost::math::weibull_distribution<Scalar> WeibMathDistType;
  typedef boost::random::weibull_distribution<Scalar> WeibRandomDistType;

  class DWeib: public BoostScalarDistribution<WeibMathDistType,
      WeibRandomDistType>
  {
  public:
    typedef DWeib SelfType;
    typedef BoostScalarDistribution<WeibMathDistType, WeibRandomDistType>
        BaseType;

  protected:
    DWeib() :
      BaseType("dweib", 2, DIST_POSITIVE, false)
    {
    }

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "dweibull";
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

#endif /* DWEIB_HPP_ */
