//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
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

/*! \file NodeArrayMonitor.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_NODEARRAYMONITOR_HPP_
#define BIIPS_NODEARRAYMONITOR_HPP_

#include "model/Monitor.hpp"
#include "common/MultiArray.hpp"
#include "common/IndexRange.hpp"

namespace Biips
{
  class NodeArray;
  class Graph;

  class NodeArrayMonitor
  {
  protected:
    String name_;
    IndexRange range_;
    Size nParticles_;
    MultiArray values_;
    MultiArray weights_;
    MultiArray ess_;
    MultiArray iterations_;
    MultiArray nodeIds_;
    MultiArray discrete_;

    template<typename StorageOrderType>
    void addObservedNode(NodeId id,
                         const IndexRange & subRange,
                         const Graph & graph);
    template<typename StorageOrderType>
    void addMonitoredNode(NodeId id,
                          const IndexRange & subRange,
                          const Monitor::Ptr & pMonitor);

  public:
    NodeArrayMonitor(const NodeArray & nodeArray,
                     const IndexRange & range,
                     const std::map<NodeId, Monitor::Ptr> & monitorsMap,
                     Size nParticles,
                     const Graph & graph);
    NodeArrayMonitor(const NodeArray & nodeArray,
                     const IndexRange & range,
                     const Monitor::Ptr & pMonitor,
                     Size nParticles,
                     const Graph & graph);

    const String & GetName() const
    {
      return name_;
    }
    const IndexRange & GetRange() const
    {
      return range_;
    }
    const MultiArray & GetNodeIds() const
    {
      return nodeIds_;
    }
    const MultiArray & GetIterations() const
    {
      return iterations_;
    }
    const MultiArray & GetESS() const
    {
      return ess_;
    }
    const MultiArray & GetWeights() const
    {
      return weights_;
    }
    const MultiArray & GetValues() const
    {
      return values_;
    }
    const MultiArray & GetDiscrete() const
    {
      return discrete_;
    }
  };

  class NodeArrayValue
  {
  protected:
    IndexRange range_;
    MultiArray value_;

    template<typename StorageOrderType>
    void addObservedNode(NodeId id,
                         const IndexRange & subRange,
                         const Graph & graph);
    template<typename StorageOrderType>
    void addMonitoredNode(NodeId id,
                          const IndexRange & subRange,
                          const Monitor::Ptr & pMonitor,
                          Size particleIndex);

  public:
    NodeArrayValue(const NodeArray & nodeArray,
                   const IndexRange & range,
                   const Monitor::Ptr & pMonitor,
                   Size particleIndex,
                   const Graph & graph);

    const MultiArray & GetValue() const
    {
      return value_;
    }
  };

}

#endif /* BIIPS_NODEARRAYMONITOR_HPP_ */
