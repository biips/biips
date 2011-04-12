//                                               -*- C++ -*-
/*! \file Monitor.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "model/Monitor.hpp"
#include "common/Error.hpp"
#include "sampler/Accumulator.hpp"

namespace Biips
{

  Bool Monitor::Contains(NodeId nodeId) const
  {
    return nodeParticles_.find(nodeId) != nodeParticles_.end();
  }


  void Monitor::AddNode(NodeId nodeId, Size nParticles, Scalar ess)
  {
    if (Contains(nodeId))
      throw LogicError("Can not add node: it has already been added in the Monitor.");

    nodeParticles_[nodeId].reserve(nParticles);
    nodeEss_.insert(std::make_pair(nodeId, ess));
  }


  void Monitor::PushParticle(NodeId nodeId, const ValArray::Ptr & pValue, Scalar weight)
  {
    if(!Contains(nodeId))
      throw LogicError("Can not push particle: Node is not monitored.");

    if (nodeParticles_[nodeId].size() == nodeParticles_[nodeId].capacity())
      throw LogicError("Can not push particle in Monitor: exceeds number of particles.");

    nodeParticles_[nodeId].push_back(std::make_pair(pValue, weight));
  }


  Scalar Monitor::GetESS(NodeId nodeId) const
  {
    if (!Contains(nodeId))
      throw LogicError("Can not get ESS: Node is not monitored.");

    return nodeEss_.at(nodeId);
  }


  void Monitor::Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n) const
  {
    if (!Contains(nodeId))
      throw LogicError("Can not accumulate: Node is not monitored.");

    featuresAcc.Init();

    for(Size i=0; i < nodeParticles_.at(nodeId).size(); ++i)
      featuresAcc.Push((*(nodeParticles_.at(nodeId)[i].first))[n], nodeParticles_.at(nodeId)[i].second);
  }

  void Monitor::Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc, const DimArray::Ptr & pDim) const
  {
    if (!Contains(nodeId))
      throw LogicError("Can not accumulate: Node is not monitored.");

    featuresAcc.Init(pDim);

    for(Size i=0; i < nodeParticles_.at(nodeId).size(); ++i)
      featuresAcc.Push(nodeParticles_.at(nodeId)[i].first, nodeParticles_.at(nodeId)[i].second);
  }
}
