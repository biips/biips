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

  Bool Monitor::Contains(NodeId nodeId) const
  {
    return particleValuesMap_.count(nodeId);
  }


  void Monitor::SetWeights(const Types<Particle>::Array & particles, Scalar ess, Scalar sumOfWeights, Scalar logNormConst)
  {
    ess_ = ess;
    sumOfWeights_ = sumOfWeights;
    logNormConst_ = logNormConst;

    weights_.resize(particles.size());
    logWeights_.resize(particles.size());
    for(Size i=0; i<particles.size(); ++i)
    {
      weights_[i] = particles[i].GetWeight();
      logWeights_[i] = particles[i].GetLogWeight();
    }
  }


  void Monitor::SetNodeValues(NodeId nodeId, const Types<Particle>::Array & particles)
  {
    if (Contains(nodeId))
      throw LogicError("Can not add node: it has already been added in the Monitor.");

    particleValuesMap_[nodeId].resize(particles.size());
    for(Size i=0; i<particles.size(); ++i)
      particleValuesMap_[nodeId][i] = particles[i].GetValue()[nodeId];
  }


  void Monitor::Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n) const
  {
    if (!Contains(nodeId))
      throw LogicError("Can not accumulate: Node is not monitored.");

    featuresAcc.Init();

    for(Size i=0; i < particleValuesMap_.at(nodeId).size(); ++i)
      featuresAcc.Push((*(particleValuesMap_.at(nodeId))[i])[n], weights_[i]);
  }


  void Monitor::Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n) const
  {
    if (!Contains(nodeId))
      throw LogicError("Can not accumulate: Node is not monitored.");

    featuresAcc.Init();

    for(Size i=0; i < particleValuesMap_.at(nodeId).size(); ++i)
      featuresAcc.Push((*(particleValuesMap_.at(nodeId))[i])[n], weights_[i]);
  }


  void Monitor::Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc, const DimArray::Ptr & pDim) const
  {
    if (!Contains(nodeId))
      throw LogicError("Can not accumulate: Node is not monitored.");

    featuresAcc.Init(pDim);

    for(Size i=0; i < particleValuesMap_.at(nodeId).size(); ++i)
      featuresAcc.Push(particleValuesMap_.at(nodeId)[i], weights_[i]);
  }
}
