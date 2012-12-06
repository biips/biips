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

/*! \file NodeArrayMonitor.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "model/NodeArrayMonitor.hpp"
#include "model/NodeArray.hpp"
#include "common/IndexRangeIterator.hpp"
#include "graph/Graph.hpp"
#include "iostream/outStream.hpp"

namespace Biips
{

  class NodeMonitor
  {
  protected:
    NodeId nodeId_;
    Size iter_;
    NodeId sampledNodeId_;
    Scalar ess_;
    ValArray weights_;
    typedef Types<ValArray::Ptr>::Array ParticleValues;
    const ParticleValues & particleValues_;
    Bool discrete_;

  public:
    NodeMonitor(NodeId id, const Monitor* & pMonitor);

    NodeId GetNodeId() const
    {
      return nodeId_;
    }
    Size GetIteration() const
    {
      return iter_;
    }
    NodeId GetSampledNode() const
    {
      return sampledNodeId_;
    }
    Scalar GetESS() const
    {
      return ess_;
    }
    Size NParticles() const
    {
      return weights_.size();
    }
    const ValArray & GetWeights() const
    {
      return weights_;
    }
    const ParticleValues & GetNodeValues() const
    {
      return particleValues_;
    }
    Bool GetDiscrete() const
    {
      return discrete_;
    }
  };

  NodeMonitor::NodeMonitor(NodeId id, const Monitor* & pMonitor) :
    nodeId_(id), iter_(pMonitor->GetIteration()),
        sampledNodeId_(pMonitor->GetSampledNodes().front()),
        ess_(pMonitor->GetNodeESS(id)), weights_(pMonitor->GetWeights()),
        particleValues_(pMonitor->GetNodeValues(id)),
        discrete_(pMonitor->GetNodeDiscrete(id))
  {
  }

  template<>
  void NodeArrayMonitor::addObservedNode<ColumnMajorOrder>(NodeId id,
                                                           const IndexRange & subRange,
                                                           const Graph & graph)
  {
    Size len = range_.Dim().Length();

    // iterate the elements of the subrange
    for (IndexRangeIterator it_sub_range(subRange); !it_sub_range.AtEnd(); it_sub_range.Next())
    {
      if (!range_.Contains(IndexRange(it_sub_range)))
        continue;
      // compute the offset of the new larger array
      Size offset = range_.GetOffset(it_sub_range);

      ess_.Values()[offset] = Scalar(nParticles_);
      iterations_.Values()[offset] = BIIPS_SIZENA;
      nodeIds_.Values()[offset] = id;
      discrete_.Values()[offset] = Scalar(graph.GetDiscrete()[id]);

      // compute the offset of the sub array
      Size sub_offset = subRange.GetOffset(it_sub_range);

      // iterate all the particles
      for (Size i = 0; i < nParticles_; ++i)
      {
        values_.Values()[i * len + offset]
            = (*(graph.GetValues()[id]))[sub_offset];
        weights_.Values()[i * len + offset] = 1.0 / nParticles_;
      }
    }
  }

  template<>
  void NodeArrayMonitor::addMonitoredNode<ColumnMajorOrder>(NodeId id,
                                                            const IndexRange & subRange,
                                                            const Monitor* pMonitor)
  {
    if (!pMonitor)
      throw LogicError("Monitor pointer is null, in NodeArrayMonitor::addMonitoredNode.");
    if (!pMonitor->Contains(id))
      throw LogicError("Node is not monitored, in NodeArrayMonitor::addMonitoredNode.");
    if (pMonitor->NParticles() != nParticles_)
      throw LogicError("Non conforming number of particles, in NodeArrayMonitor::addMonitoredNode");

    NodeMonitor node_monitor(id, pMonitor);
    Size len = range_.Dim().Length();

    // iterate the elements of the subrange
    for (IndexRangeIterator it_sub_range(subRange); !it_sub_range.AtEnd(); it_sub_range.Next())
    {
      if (!range_.Contains(IndexRange(it_sub_range)))
        continue;
      // compute the offset of the new larger array
      Size offset = range_.GetOffset(it_sub_range);

      ess_.Values()[offset] = node_monitor.GetESS();
      iterations_.Values()[offset] = node_monitor.GetIteration();
      nodeIds_.Values()[offset] = id;
      discrete_.Values()[offset] = Scalar(node_monitor.GetDiscrete());

      // compute the offset of the sub array
      Size sub_offset = subRange.GetOffset(it_sub_range);

      // iterate all the particles
      for (Size i = 0; i < nParticles_; ++i)
      {
        values_.Values()[i * len + offset]
            = (*(node_monitor.GetNodeValues()[i]))[sub_offset];
        weights_.Values()[i * len + offset] = (node_monitor.GetWeights()[i]);
      }
    }
  }

  NodeArrayMonitor::NodeArrayMonitor(const NodeArray & nodeArray,
                                     const IndexRange & range,
                                     const std::map<NodeId, Monitor*> & monitorsMap,
                                     Size nParticles,
                                     const Graph & graph) :
    name_(nodeArray.Name()), range_(range), nParticles_(nParticles),
        ess_(range.DimPtr(), ValArray::Ptr(new ValArray(range.Length(),
                                                        BIIPS_REALNA))),
        iterations_(range.DimPtr(), ValArray::Ptr(new ValArray(range.Length(),
                                                               BIIPS_REALNA))),
        nodeIds_(range.DimPtr(), ValArray::Ptr(new ValArray(range.Length(),
                                                            BIIPS_REALNA))),
        discrete_(range.DimPtr(), ValArray::Ptr(new ValArray(range.Length(),
                                                             Scalar(false))))
  {
    if (!nodeArray.Range().Contains(range_))
      throw LogicError(String("NodeArrayMonitor: range ") + print(range_)
          + " is not contained in variable " + name_);

    // dimensions
    DimArray::Ptr p_dim_particles(new DimArray(range_.Dim()));
    p_dim_particles->push_back(nParticles_);

    // allocate memory
    ValArray::Ptr p_values_val(new ValArray(p_dim_particles->Length(),
                                            BIIPS_REALNA));
    ValArray::Ptr p_weights_val(new ValArray(p_dim_particles->Length(),
                                             BIIPS_REALNA));
    values_.SetPtr(p_dim_particles, p_values_val);
    weights_.SetPtr(p_dim_particles, p_weights_val);

    boost::bimap<NodeId, IndexRange>::const_iterator it_bimap =
        nodeArray.NodeIdRangeBimap().begin();
    // iterate over all the subnodes
    for (; it_bimap != nodeArray.NodeIdRangeBimap().end(); ++it_bimap)
    {
      NodeId id = it_bimap->left;
      const IndexRange & sub_range = it_bimap->right;
      if (!range_.Overlaps(sub_range))
        continue;

      if (graph.GetObserved()[id])
        addObservedNode<StorageOrder> (id, sub_range, graph);
      else
      {
        if (!monitorsMap.count(id))
          throw LogicError("Node is not monitored, in NodeArrayMonitor.");
        addMonitoredNode<StorageOrder> (id, sub_range, monitorsMap.at(id));
      }
    }
  }

  NodeArrayMonitor::NodeArrayMonitor(const NodeArray & nodeArray,
                                     const IndexRange & range,
                                     const Monitor* pMonitor,
                                     Size nParticles,
                                     const Graph & graph) :
    name_(nodeArray.Name()), range_(range), nParticles_(nParticles),
        ess_(range.DimPtr(), ValArray::Ptr(new ValArray(range.Length(),
                                                        BIIPS_REALNA))),
        iterations_(range.DimPtr(), ValArray::Ptr(new ValArray(range.Length(),
                                                               BIIPS_REALNA))),
        nodeIds_(range.DimPtr(), ValArray::Ptr(new ValArray(range.Length(),
                                                            BIIPS_REALNA))),
        discrete_(range.DimPtr(), ValArray::Ptr(new ValArray(range.Length(),
                                                             Scalar(false))))
  {
    if (!nodeArray.Range().Contains(range_))
      throw LogicError(String("NodeArrayMonitor: range ") + print(range_)
          + " is not contained in variable " + name_);

    // dimensions
    DimArray::Ptr p_dim_particles(new DimArray(range_.Dim()));
    p_dim_particles->push_back(nParticles_);

    // allocate memory
    ValArray::Ptr p_values_val(new ValArray(p_dim_particles->Length(),
                                            BIIPS_REALNA));
    ValArray::Ptr p_weights_val(new ValArray(p_dim_particles->Length(),
                                             BIIPS_REALNA));
    values_.SetPtr(p_dim_particles, p_values_val);
    weights_.SetPtr(p_dim_particles, p_weights_val);

    boost::bimap<NodeId, IndexRange>::const_iterator it_bimap =
        nodeArray.NodeIdRangeBimap().begin();

    // iterate over all the subnodes
    for (; it_bimap != nodeArray.NodeIdRangeBimap().end(); ++it_bimap)
    {
      NodeId id = it_bimap->left;
      const IndexRange & sub_range = it_bimap->right;
      if (!range_.Overlaps(sub_range))
        continue;

      if (graph.GetObserved()[id])
        addObservedNode<StorageOrder> (id, sub_range, graph);
      else
        addMonitoredNode<StorageOrder> (id, sub_range, pMonitor);
    }
  }

  template<>
  void NodeArrayValue::addObservedNode<ColumnMajorOrder>(NodeId id,
                                                        const IndexRange & subRange,
                                                        const Graph & graph)
  {
    // iterate the elements of the subrange
    for (IndexRangeIterator it_sub_range(subRange); !it_sub_range.AtEnd(); it_sub_range.Next())
    {
      if (!range_.Contains(IndexRange(it_sub_range)))
        continue;
      // compute the offset of the new larger array
      Size offset = range_.GetOffset(it_sub_range);

      // compute the offset of the sub array
      Size sub_offset = subRange.GetOffset(it_sub_range);

      value_.Values()[offset] = (*(graph.GetValues()[id]))[sub_offset];
    }
  }

  template<>
  void NodeArrayValue::addMonitoredNode<ColumnMajorOrder>(NodeId id,
                                                        const IndexRange & subRange,
                                                        const Monitor* pMonitor,
                                                        Size particleIndex)
  {
    if (!pMonitor)
      throw LogicError("Monitor::Ptr is null, in NodeArrayMonitor::addMonitoredNode.");
    if (!pMonitor->Contains(id))
      throw LogicError("Node is not monitored, in NodeArrayMonitor::addMonitoredNode.");

    NodeMonitor node_monitor(id, pMonitor);

    // iterate the elements of the subrange
    for (IndexRangeIterator it_sub_range(subRange); !it_sub_range.AtEnd(); it_sub_range.Next())
    {
      if (!range_.Contains(IndexRange(it_sub_range)))
        continue;
      // compute the offset of the new larger array
      Size offset = range_.GetOffset(it_sub_range);

      // compute the offset of the sub array
      Size sub_offset = subRange.GetOffset(it_sub_range);

      // iterate all the particles
      value_.Values()[offset]
          = (*(node_monitor.GetNodeValues()[particleIndex]))[sub_offset];
    }
  }

  NodeArrayValue::NodeArrayValue(const NodeArray & nodeArray,
                                 const IndexRange & range,
                                 const Monitor* pMonitor,
                                 Size particleIndex,
                                 const Graph & graph) :
    range_(range)
  {
    if (!nodeArray.Range().Contains(range_))
      throw LogicError(String("NodeArrayMonitor: range ") + print(range_)
          + " is not contained in variable " + nodeArray.Name());

    // allocate memory
    ValArray::Ptr
        p_values_val(new ValArray(range_.Dim().Length(), BIIPS_REALNA));
    value_.SetPtr(range_.DimPtr(), p_values_val);

    boost::bimap<NodeId, IndexRange>::const_iterator it_bimap =
        nodeArray.NodeIdRangeBimap().begin();

    // iterate over all the subnodes
    for (; it_bimap != nodeArray.NodeIdRangeBimap().end(); ++it_bimap)
    {
      NodeId id = it_bimap->left;
      const IndexRange & sub_range = it_bimap->right;
      if (!range_.Overlaps(sub_range))
        continue;

      if (graph.GetObserved()[id])
        addObservedNode<StorageOrder> (id, sub_range, graph);
      else
        addMonitoredNode<StorageOrder> (id, sub_range, pMonitor, particleIndex);
    }
  }

}
