//                                               -*- C++ -*-
/*! \file Monitor.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_MONITOR_HPP_
#define BIIPS_MONITOR_HPP_

#include "common/Types.hpp"
#include "common/ValArray.hpp"
#include "common/DimArray.hpp"
#include <map>

namespace Biips
{

  class ScalarAccumulator;
  class DiscreteScalarAccumulator;
  class ElementAccumulator;
  class Particle;

  class Monitor
  {
  public:
    typedef Monitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef Types<ValArray::Ptr>::Array ParticleValue;

    Size t_;
    NodeId sampledNodeId_;
    std::map<NodeId, Types<ValArray::Ptr>::Array> particleValuesMap_;
    ValArray logWeights_;
    ValArray weights_;
    Scalar ess_;
    Scalar sumOfWeights_;
    Scalar logNormConst_;

  public:
    Monitor(Size t, NodeId sampledNodeId) : t_(t), sampledNodeId_(sampledNodeId) {};

    Bool Contains(NodeId nodeId) const;

    void SetWeights(const Types<Particle>::Array & particles, Scalar ess, Scalar sumOfWeights, Scalar logNormConst);

    void SetNodeValues(NodeId nodeId, const Types<Particle>::Array & particles);

    Size NParticles() const { return weights_.size(); }

    Size GetTime() const { return t_; };
    NodeId GetSampledNode() const { return sampledNodeId_; };
    Scalar GetESS() const { return ess_; };
    Scalar GetSumOfWeights() const { return sumOfWeights_; };
    const ValArray & GetWeights() const { return weights_; };
    ValArray & Weights() { return weights_; };
    const ValArray & GetLogWeights() const { return logWeights_; };
    const Types<ValArray::Ptr>::Array & GetNodeValues(NodeId nodeId) const { return particleValuesMap_.at(nodeId); };

    void Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc, const DimArray::Ptr & pDim) const;

    void ClearNode(NodeId nodeId) { particleValuesMap_.at(nodeId).clear(); };
    void Clear() { particleValuesMap_.clear(); logWeights_.clear(); weights_.clear(); };

    virtual ~Monitor() {};
  };

}

#endif /* BIIPS_MONITOR_HPP_ */
