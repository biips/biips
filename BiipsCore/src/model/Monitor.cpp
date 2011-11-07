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
#include "sampler/Particle.hpp"
#include "sampler/Accumulator.hpp"

namespace Biips
{

  void Monitor::checkWeights() const
  {
    if (!weightsSet_)
      throw LogicError("Can not access monitor weights: weights not set.");
  }

  void Monitor::checkSwapped() const
  {
    if (weightsSwapped_)
      throw LogicError("Can not access monitor weights: swapped.");
  }


  Bool Monitor::Contains(NodeId nodeId) const
  {
    return particleValuesMap_.count(nodeId);
  }


  Size Monitor::GetNodeSamplingIteration(NodeId id) const
  {
    if (!Contains(id))
      throw LogicError("Can not get node sampling iteration: node is not monitored.");

    return nodeIterationMap_.at(id);
  }


  Bool Monitor::HasIterationESS(Size iter) const
  {
    return iterationEssMap_.find(iter) != iterationEssMap_.end();
  }


  Scalar Monitor::GetNodeESS(NodeId nodeId) const
  {
    if (!Contains(nodeId))
      throw LogicError("Can not get node ess: Node is not monitored.");

    Size iter = GetNodeSamplingIteration(nodeId);

    if (!HasIterationESS(iter))
      throw LogicError("Can not get node ess: ess not set for this iteration.");

    return iterationEssMap_.at(iter);
  }


  void Monitor::SetIterationESS(Size iter, Scalar ess)
  {
    if (HasIterationESS(iter))
      throw LogicError("Can not set node ess: already set.");

    iterationEssMap_[iter] = ess;
  }


  Types<NodeId>::Array Monitor::GetNodes() const
  {
    Types<NodeId>::Array nodes(particleValuesMap_.size());
    std::map<NodeId, Types<ValArray::Ptr>::Array>::const_iterator it = particleValuesMap_.begin();
    for (Size i=0; it != particleValuesMap_.end(); ++it)
    {
      nodes[i] = it->first;
      ++i;
    }
    return nodes;
  }


  void Monitor::Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n) const
  {
    checkWeights();
    checkSwapped();

    if (!Contains(nodeId))
      throw LogicError("Can not accumulate: Node is not monitored.");

    featuresAcc.Init();

    for(Size i=0; i < particleValuesMap_.at(nodeId).size(); ++i)
      featuresAcc.Push((*(particleValuesMap_.at(nodeId))[i])[n], weights_[i]);
  }


  void Monitor::Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n) const
  {
    checkWeights();
    checkSwapped();

    if (!Contains(nodeId))
      throw LogicError("Can not accumulate: Node is not monitored.");

    featuresAcc.Init();

    for(Size i=0; i < particleValuesMap_.at(nodeId).size(); ++i)
      featuresAcc.Push((*(particleValuesMap_.at(nodeId))[i])[n], weights_[i]);
  }


  void Monitor::Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc, const DimArray::Ptr & pDim) const
  {
    checkWeights();
    checkSwapped();

    if (!Contains(nodeId))
      throw LogicError("Can not accumulate: Node is not monitored.");

    featuresAcc.Init(pDim);

    for(Size i=0; i < particleValuesMap_.at(nodeId).size(); ++i)
      featuresAcc.Push(particleValuesMap_.at(nodeId)[i], weights_[i]);
  }


  void FilterMonitor::Init(const Types<Particle>::Array & particles, Scalar ess, Scalar sumOfWeights, Bool resampled, Scalar logNormConst)
  {
    checkSwapped();

    weights_.resize(particles.size());
    for(Size i=0; i<particles.size(); ++i)
      weights_[i] = particles[i].Weight();

    weightsSet_ = true;
    
    ess_ = ess;
    sumOfWeights_ = sumOfWeights;
    resampled_ = resampled;
    logNormConst_ = logNormConst;
  }


  void FilterMonitor::AddNode(NodeId nodeId, const Types<Particle>::Array & particles, Size iter)
  {
    if (Contains(nodeId))
      throw LogicError("Can not add node: it has already been added in the Monitor.");

    particleValuesMap_[nodeId].resize(particles.size());
    for(Size i=0; i<particles.size(); ++i)
      particleValuesMap_[nodeId][i] = particles[i].GetValue()[nodeId];

    nodeIterationMap_[nodeId] = iter;
  }


  void SmoothMonitor::Init(const ValArray & weights, Scalar ess, Scalar sumOfWeights)
  {
    checkSwapped();
    weights_.assign(weights.begin(), weights.end());
    
    weightsSet_ = true;
    ess_ = ess;
    sumOfWeights_ = sumOfWeights;
  }


  void SmoothMonitor::AddNode(NodeId nodeId, const Monitor & filterMonitor)
  {
    particleValuesMap_[nodeId].assign(filterMonitor.GetNodeValues(nodeId).begin(), filterMonitor.GetNodeValues(nodeId).end());

    nodeIterationMap_[nodeId] = filterMonitor.GetNodeSamplingIteration(nodeId);
  };
}
