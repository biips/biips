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

/*! \file StochasticNode.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_STOCHASTICNODE_HPP_
#define BIIPS_STOCHASTICNODE_HPP_

#include "Node.hpp"
#include "distribution/Distribution.hpp"

namespace Biips
{

  class StochasticNode: public Node
  {
  public:
    typedef StochasticNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Types<SelfType>::IteratorPair IteratorPair;

  protected:
    typedef NumArray::StorageType StorageType;

    Distribution::Ptr pPrior_;
    NodeId lowerNodeId_;
    NodeId upperNodeId_;

  public:
    const Distribution::Ptr & PriorPtr() const
    {
      return pPrior_;
    }
    const String & PriorName() const
    {
      return pPrior_->Name();
    }

    void Sample(ValArray & values,
                const NumArray::Array & paramValues,
                const NumArray::Pair & boundValues,
                Rng & rng) const
    {
      pPrior_->Sample(values, paramValues, boundValues, rng);
    }
    Scalar LogPriorDensity(const NumArray & x,
                           const NumArray::Array & paramValues,
                           const NumArray::Pair & boundValues) const
    {
      return pPrior_->LogDensity(x, paramValues, boundValues);
    }
    void FixedUnboundedSupport(ValArray & lower,
                          ValArray & upper,
                          const NumArray::Array & fixedParamValues) const
    {
      pPrior_->FixedUnboundedSupport(lower, upper, fixedParamValues);
    }

    NodeId Lower() const
    {
      return lowerNodeId_;
    }

    NodeId Upper() const
    {
      return upperNodeId_;
    }

    Bool IsLowerBounded() const
    {
      return lowerNodeId_ != NULL_NODEID;
    }
    Bool IsUpperBounded() const
    {
      return upperNodeId_ != NULL_NODEID;
    }
    Bool IsBounded() const
    {
      return IsLowerBounded() || IsUpperBounded();
    }

    StochasticNode(const DimArray::Ptr pDim,
                   const Distribution::Ptr & pPrior,
                   const Types<NodeId>::Array & parameters,
                   NodeId lower = NULL_NODEID,
                   NodeId upper = NULL_NODEID);

    virtual ~StochasticNode()
    {
    }
  };

} /* namespace Biips */

#endif /* BIIPS_STOCHASTICNODE_HPP_ */
