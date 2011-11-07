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

  public:
    NodeMonitor(NodeId id, const Monitor::Ptr & pMonitor);

    NodeId GetNodeId() const { return nodeId_; }
    Size GetIteration() const { return iter_; };
    NodeId GetSampledNode() const { return sampledNodeId_; };
    Scalar GetESS() const { return ess_; };
    Size NParticles() const { return weights_.size(); }
    const ValArray & GetWeights() const { return weights_; }
    const ParticleValues & GetNodeValues() const { return particleValues_; }
  };


  NodeMonitor::NodeMonitor(NodeId id, const Monitor::Ptr & pMonitor)
    : nodeId_(id), iter_(pMonitor->GetIteration()),
      sampledNodeId_(pMonitor->GetSampledNodes().front()), ess_(pMonitor->GetNodeESS(id)),
      weights_(pMonitor->GetWeights()), particleValues_(pMonitor->GetNodeValues(id))
  {
  }


  // TODO: check StorageOrder, check monitors
  NodeArrayMonitor::NodeArrayMonitor(const NodeArray & nodeArray, const std::map<NodeId, Monitor::Ptr> & monitorsMap)
  : name_(nodeArray.Name()), range_(nodeArray.Range()),
    ess_(nodeArray.Range().Dim(), BIIPS_REALNA), iterations_(nodeArray.Range().Dim(), BIIPS_REALNA), nodeIds_(nodeArray.Range().Dim(), BIIPS_REALNA)
  {
    // dimensions
    Size len = range_.Dim().Length();
    if (!monitorsMap.begin()->second)
      throw LogicError("Monitor::Ptr is null.");
    Size n_particles = monitorsMap.begin()->second->NParticles();
    DimArray dim_particles = range_.Dim();
    dim_particles.push_back(n_particles);

    // allocate memory
    values_.Alloc(dim_particles).AllocValue(BIIPS_REALNA);
    weights_.Alloc(dim_particles).AllocValue(BIIPS_REALNA);

    boost::bimap<NodeId, IndexRange>::const_iterator it_bimap = nodeArray.NodeIdRangeBimap().begin();

    // iterate over all the subnodes
    for(; it_bimap != nodeArray.NodeIdRangeBimap().end(); ++it_bimap)
    {
      NodeId id = it_bimap->left;
      const IndexRange & sub_range = it_bimap->right;

      if (!monitorsMap.count(id))
        throw LogicError("Node is not monitored, in NodeArrayMonitor.");
      if (!monitorsMap.at(id))
        throw LogicError("Monitor::Ptr is null, in NodeArrayMonitor.");
      if (!monitorsMap.at(id)->Contains(id))
        throw LogicError("Node is not monitored, in NodeArrayMonitor.");
      NodeMonitor node_monitor(id, monitorsMap.at(id));

      // iterate the elements of the subrange
      for (IndexRangeIterator it_sub_range(sub_range); !it_sub_range.AtEnd(); it_sub_range.Next())
      {
        // compute the offset of the new larger array
        Size offset = range_.GetOffset(it_sub_range);

        ess_.Values()[offset] = node_monitor.GetESS();
        iterations_.Values()[offset] = node_monitor.GetIteration();
        nodeIds_.Values()[offset] = id;

        // compute the offset of the sub array
        Size sub_offset = sub_range.GetOffset(it_sub_range);

        // iterate all the particles
        for (Size i=0; i<n_particles; ++i)
        {
          // TODO: check StorageOrder
          values_.Values()[i*len+offset] = (*(node_monitor.GetNodeValues()[i]))[sub_offset];
          weights_.Values()[i*len+offset] = (node_monitor.GetWeights()[i]);
        }
      }
    }
  }


  // TODO: check StorageOrder, check monitor
  NodeArrayMonitor::NodeArrayMonitor(const NodeArray & nodeArray, const Monitor::Ptr & pMonitor)
  : name_(nodeArray.Name()), range_(nodeArray.Range()),
    ess_(nodeArray.Range().Dim(), BIIPS_REALNA), iterations_(nodeArray.Range().Dim(), BIIPS_REALNA), nodeIds_(nodeArray.Range().Dim(), BIIPS_REALNA)
  {
    // dimensions
    Size len = range_.Dim().Length();
    if (!pMonitor)
      throw LogicError("Monitor::Ptr is null.");
    Size n_particles = pMonitor->NParticles();
    DimArray dim_particles = range_.Dim();
    dim_particles.push_back(n_particles);

    // allocate memory
    values_.Alloc(dim_particles).AllocValue(BIIPS_REALNA);
    weights_.Alloc(dim_particles).AllocValue(BIIPS_REALNA);

    boost::bimap<NodeId, IndexRange>::const_iterator it_bimap = nodeArray.NodeIdRangeBimap().begin();

    // iterate over all the subnodes
    for(; it_bimap != nodeArray.NodeIdRangeBimap().end(); ++it_bimap)
    {
      NodeId id = it_bimap->left;
      const IndexRange & sub_range = it_bimap->right;

      if (!pMonitor->Contains(id))
        throw LogicError("Node is not monitored, in NodeArrayMonitor.");
      NodeMonitor node_monitor(id, pMonitor);

      // iterate the elements of the subrange
      for (IndexRangeIterator it_sub_range(sub_range); !it_sub_range.AtEnd(); it_sub_range.Next())
      {
        // compute the offset of the new larger array
        Size offset = range_.GetOffset(it_sub_range);

        ess_.Values()[offset] = node_monitor.GetESS();
        iterations_.Values()[offset] = node_monitor.GetIteration();
        nodeIds_.Values()[offset] = id;

        // compute the offset of the sub array
        Size sub_offset = sub_range.GetOffset(it_sub_range);

        // iterate all the particles
        for (Size i=0; i<n_particles; ++i)
        {
          // TODO: check StorageOrder
          values_.Values()[i*len+offset] = (*(node_monitor.GetNodeValues()[i]))[sub_offset];
          weights_.Values()[i*len+offset] = (node_monitor.GetWeights()[i]);
        }
      }
    }
  }

}
