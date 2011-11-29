//                                               -*- C++ -*-
/*! \file NodeArrayMonitor.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "model/NodeArrayMonitor.hpp"
#include "model/NodeArray.hpp"
#include "common/IndexRangeIterator.hpp"
#include "graph/Graph.hpp"

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
    NodeMonitor(NodeId id, const Monitor::Ptr & pMonitor);

    NodeId GetNodeId() const { return nodeId_; }
    Size GetIteration() const { return iter_; };
    NodeId GetSampledNode() const { return sampledNodeId_; };
    Scalar GetESS() const { return ess_; };
    Size NParticles() const { return weights_.size(); }
    const ValArray & GetWeights() const { return weights_; }
    const ParticleValues & GetNodeValues() const { return particleValues_; }
    Bool GetDiscrete() const { return discrete_; }
  };


  NodeMonitor::NodeMonitor(NodeId id, const Monitor::Ptr & pMonitor)
    : nodeId_(id), iter_(pMonitor->GetIteration()),
      sampledNodeId_(pMonitor->GetSampledNodes().front()), ess_(pMonitor->GetNodeESS(id)),
      weights_(pMonitor->GetWeights()), particleValues_(pMonitor->GetNodeValues(id)),
      discrete_(pMonitor->GetNodeDiscrete(id))
  {
  }


  template <>
  void NodeArrayMonitor::addObservedNode<ColumnMajorOrder>(NodeId id, const IndexRange & subRange, const Graph & graph)
  {
    Size len = range_.Dim().Length();

    // iterate the elements of the subrange
    for (IndexRangeIterator it_sub_range(subRange); !it_sub_range.AtEnd(); it_sub_range.Next())
    {
      // compute the offset of the new larger array
      Size offset = range_.GetOffset(it_sub_range);

      ess_.Values()[offset] = Scalar(nParticles_);
      iterations_.Values()[offset] = BIIPS_SIZENA;
      nodeIds_.Values()[offset] = id;
      discrete_.Values()[offset] = Scalar(graph.GetDiscrete()[id]);

      // compute the offset of the sub array
      Size sub_offset = subRange.GetOffset(it_sub_range);

      // iterate all the particles
      for (Size i=0; i<nParticles_; ++i)
      {
        // TODO: check StorageOrder
        values_.Values()[i*len+offset] = (*(graph.GetValues()[id]))[sub_offset];
        weights_.Values()[i*len+offset] = 1.0/nParticles_;
      }
    }
  }


  template <>
  void NodeArrayMonitor::addMonitoredNode<ColumnMajorOrder>(NodeId id, const IndexRange & subRange, const Monitor::Ptr & pMonitor)
  {
    if (!pMonitor)
      throw LogicError("Monitor::Ptr is null, in NodeArrayMonitor::addMonitoredNode.");
    if (!pMonitor->Contains(id))
      throw LogicError("Node is not monitored, in NodeArrayMonitor::addMonitoredNode.");
    if (pMonitor->NParticles() != nParticles_)
      throw LogicError("Non conforming number of particles, in NodeArrayMonitor::addMonitoredNode");

    NodeMonitor node_monitor(id, pMonitor);
    Size len = range_.Dim().Length();

    // iterate the elements of the subrange
    for (IndexRangeIterator it_sub_range(subRange); !it_sub_range.AtEnd(); it_sub_range.Next())
    {
      // compute the offset of the new larger array
      Size offset = range_.GetOffset(it_sub_range);

      ess_.Values()[offset] = node_monitor.GetESS();
      iterations_.Values()[offset] = node_monitor.GetIteration();
      nodeIds_.Values()[offset] = id;
      discrete_.Values()[offset] = Scalar(node_monitor.GetDiscrete());

      // compute the offset of the sub array
      Size sub_offset = subRange.GetOffset(it_sub_range);

      // iterate all the particles
      for (Size i=0; i<nParticles_; ++i)
      {
        // TODO: check StorageOrder
        values_.Values()[i*len+offset] = (*(node_monitor.GetNodeValues()[i]))[sub_offset];
        weights_.Values()[i*len+offset] = (node_monitor.GetWeights()[i]);
      }
    }
  }


  // TODO: check StorageOrder, check monitors
  NodeArrayMonitor::NodeArrayMonitor(const NodeArray & nodeArray, const std::map<NodeId, Monitor::Ptr> & monitorsMap, Size nParticles, const Graph & graph)
  : name_(nodeArray.Name()), range_(nodeArray.Range()), nParticles_(nParticles),
    ess_(nodeArray.Range().Dim(), BIIPS_REALNA),
    iterations_(nodeArray.Range().Dim(), BIIPS_REALNA),
    nodeIds_(nodeArray.Range().Dim(), BIIPS_REALNA),
    discrete_(nodeArray.Range().Dim(), Scalar(false))
  {
    // dimensions
    DimArray dim_particles = range_.Dim();
    dim_particles.push_back(nParticles_);

    // allocate memory
    values_.Alloc(dim_particles).AllocValue(BIIPS_REALNA);
    weights_.Alloc(dim_particles).AllocValue(BIIPS_REALNA);

    boost::bimap<NodeId, IndexRange>::const_iterator it_bimap = nodeArray.NodeIdRangeBimap().begin();
    // iterate over all the subnodes
    for(; it_bimap != nodeArray.NodeIdRangeBimap().end(); ++it_bimap)
    {
      NodeId id = it_bimap->left;
      const IndexRange & sub_range = it_bimap->right;

      if (graph.GetObserved()[id])
        addObservedNode<StorageOrder>(id, sub_range, graph);
      else
      {
        if (!monitorsMap.count(id))
          throw LogicError("Node is not monitored, in NodeArrayMonitor.");
        addMonitoredNode<StorageOrder>(id, sub_range, monitorsMap.at(id));
      }
    }
  }


  // TODO: check StorageOrder, check monitor
  NodeArrayMonitor::NodeArrayMonitor(const NodeArray & nodeArray, const Monitor::Ptr & pMonitor, Size nParticles, const Graph & graph)
  : name_(nodeArray.Name()), range_(nodeArray.Range()), nParticles_(nParticles),
    ess_(nodeArray.Range().Dim(), BIIPS_REALNA),
    iterations_(nodeArray.Range().Dim(), BIIPS_REALNA),
    nodeIds_(nodeArray.Range().Dim(), BIIPS_REALNA),
    discrete_(nodeArray.Range().Dim(), Scalar(false))
  {
    // dimensions
    DimArray dim_particles = range_.Dim();
    dim_particles.push_back(nParticles_);

    // allocate memory
    values_.Alloc(dim_particles).AllocValue(BIIPS_REALNA);
    weights_.Alloc(dim_particles).AllocValue(BIIPS_REALNA);

    boost::bimap<NodeId, IndexRange>::const_iterator it_bimap = nodeArray.NodeIdRangeBimap().begin();

    // iterate over all the subnodes
    for(; it_bimap != nodeArray.NodeIdRangeBimap().end(); ++it_bimap)
    {
      NodeId id = it_bimap->left;
      const IndexRange & sub_range = it_bimap->right;

      if (graph.GetObserved()[id])
        addObservedNode<StorageOrder>(id, sub_range, graph);
      else
        addMonitoredNode<StorageOrder>(id, sub_range, pMonitor);
    }
  }

}
