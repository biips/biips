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

/*! \file BackwardSmoother.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_BACKWARDSMOOTHER_HPP_
#define BIIPS_BACKWARDSMOOTHER_HPP_

#include "graph/Graph.hpp"
#include "model/Monitor.hpp"

namespace Biips
{

  class Graph;

  class BackwardSmoother
  {
  public:
    typedef BackwardSmoother SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    const Graph & graph_;
    Types<Monitor*>::Array filterMonitors_;
    //    ValArray logWeights_;
    ValArray weights_;
    Scalar sumOfWeights_;
    Scalar ess_;
    Size iter_;
    Bool initialized_;
    Types<Size>::Array nodeIterations_;

    void sumOfWeightsAndEss();
    Monitor * getParentFilterMonitor(NodeId id);

  public:
    BackwardSmoother(const Graph & graph,
                     const Types<Monitor *>::Array & filterMonitors,
                     const Types<Size>::Array & nodeIterations);

    void Initialize();
    void IterateBack();

    Bool Initialized() const
    {
      return initialized_;
    }
    ;

    Size Iteration() const
    {
      return iter_;
    }
    Bool AtEnd() const
    {
      return filterMonitors_.size() == 1;
    }
    const Types<NodeId>::Array & UpdatedNodes() const
    {
      return filterMonitors_.back()->GetSampledNodes();
    }
    ;
    Size GetNodeSamplingIteration(NodeId nodeId) const;

    Scalar GetNodeESS(NodeId nodeId) const;

    void
    Accumulate(NodeId nodeId, Accumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId,
                    DiscreteAccumulator & featuresAcc,
                    Size n = 0) const;
    void Accumulate(NodeId nodeId, ArrayAccumulator & featuresAcc) const;

    void InitMonitor(SmoothMonitor & monitor) const;
    void MonitorNode(NodeId nodeId, SmoothMonitor & monitor) const;
  };

}

#endif /* BIIPS_BACKWARDSMOOTHER_HPP_ */
