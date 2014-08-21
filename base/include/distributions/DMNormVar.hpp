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

/*! \file DMNormVar.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DMNORMVAR_HPP_
#define BIIPS_DMNORMVAR_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{
  class DMNormVar: public Distribution
  {
  protected:
    typedef DMNormVar SelfType;

    DMNormVar() :
      Distribution("dmnormvar", 2)
    {
    }

    virtual Bool
    checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual void sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const;
    virtual Scalar logDensity(const NumArray & x,
                              const NumArray::Array & paramValues,
                              const NumArray::Pair & boundValues) const;
    virtual void
    fixedUnboundedSupport(ValArray & lower,
                     ValArray & upper,
                     const NumArray::Array & paramValues) const;

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;
    virtual Bool IsSupportFixed(const Flags & fixmask) const
    {
      return true;
    }

    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

} /* namespace Biips */

#endif /* BIIPS_DMNORMVAR_HPP_ */
