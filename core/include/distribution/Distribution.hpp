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

/*! \file Distribution.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DISTRIBUTION_HPP_
#define BIIPS_DISTRIBUTION_HPP_

#include "common/Common.hpp"
#include "common/Table.hpp"

namespace Biips
{

  const String UNDEF_DIST = "undef";

  class Distribution
  {
  protected:
    const String name_;
    const Size nParam_;

    Distribution(const String & name, Size nParam) :
      name_(name), nParam_(nParam)
    {
    }

    virtual Bool
        checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const = 0;
    virtual DimArray
        dim(const Types<DimArray::Ptr>::Array & paramDims) const = 0;
    virtual void sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const = 0;
    virtual Scalar logDensity(const NumArray & x,
                              const NumArray::Array & paramValues,
                              const NumArray::Pair & boundValues) const = 0;
    virtual void fixedUnboundedSupport(ValArray & lower,
                                  ValArray & upper,
                                  const NumArray::Array & fixedParamValues) const = 0;

  public:
    typedef Distribution SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    String const & Name() const
    {
      return name_;
    }
    virtual String Alias() const
    {
      return "";
    }
    Size NParam() const
    {
      return nParam_;
    }

    Bool CheckNParam(Size n) const
    {
      return n == nParam_;
    }

    Bool CheckParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;

    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const = 0;

    virtual Bool
        CheckDensityParamValues(const NumArray & x,
                                const NumArray::Array & paramValues) const;

    DimArray Dim(const Types<DimArray::Ptr>::Array & paramDims) const;

    void Sample(ValArray & values,
                const NumArray::Array & paramValues,
                const NumArray::Pair & boundValues,
                Rng & rng) const;

    // TODO use PDFType argument to optimize calculations : cf JAGS Distribution.h
    Scalar LogDensity(const NumArray & x,
                      const NumArray::Array & paramValues,
                      const NumArray::Pair & boundValues) const;

    void FixedUnboundedSupport(ValArray & lower,
                          ValArray & upper,
                          const NumArray::Array & paramValues) const;

    // is unbounded support fixed
    virtual Bool IsSupportFixed(const Flags & fixmask) const = 0;

    virtual Bool CanBound() const
    {
      return false;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const
    {
      return false;
    }
    virtual Bool CheckParamDiscrete(const Flags & mask) const
    {
      return true;
    }
    virtual Bool IsLocationParameter(Size i) const
    {
      return false;
    }
    virtual Bool IsScaleParameter(Size i) const
    {
      return false;
    }
    virtual Size DegreeOfFreedom(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return Dim(paramDims).Length();
    }

    virtual ~Distribution()
    {
    }
  };

  typedef Table<Distribution> DistributionTable;
} /* namespace Biips */

#endif /* BIIPS_DISTRIBUTION_HPP_ */
