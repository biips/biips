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
    Size t_;
    NodeId sampledNodeId_;
    ValArray weights_;
    Scalar ess_;
    Scalar sumOfWeights_;
    typedef Types<ValArray::Ptr>::Array ParticleValues;
    std::map<NodeId, ParticleValues> particleValuesMap_;

    Bool weightsSwapped_;
    void checkSwapped() const;

  public:
    Monitor(Size t, NodeId sampledNodeId) : t_(t), sampledNodeId_(sampledNodeId), weightsSwapped_(false) {};
    virtual ~Monitor() {};

    Bool Contains(NodeId nodeId) const;
    Types<NodeId>::Array GetNodes() const;

    Size NParticles() const { return weights_.size(); }
    Size GetTime() const { return t_; };
    NodeId GetSampledNode() const { return sampledNodeId_; };
    Scalar GetESS() const { return ess_; };
    Scalar GetSumOfWeights() const { return sumOfWeights_; };
    const ValArray & GetWeights() const { checkSwapped(); return weights_; };

    void SwapWeights(ValArray::BaseType & vec) { weights_.swap(vec); weightsSwapped_ = !weightsSwapped_; };

    const ParticleValues & GetNodeValues(NodeId nodeId) const { return particleValuesMap_.at(nodeId); };

    void Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc, const DimArray::Ptr & pDim) const;

    void ClearNode(NodeId nodeId) { particleValuesMap_.at(nodeId).clear(); };
  };


  class FilterMonitor : public Monitor
  {
  public:
    typedef FilterMonitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Monitor BaseType;

  protected:
    Scalar logNormConst_;

  public:
    FilterMonitor(Size t, NodeId sampledNodeId) : BaseType(t, sampledNodeId) {};
    virtual ~FilterMonitor() {};

    void SetWeights(const Types<Particle>::Array & particles, Scalar ess, Scalar sumOfWeights, Scalar logNormConst);
    void SetNodeValues(NodeId nodeId, const Types<Particle>::Array & particles);
  };


  class SmoothMonitor : public Monitor
  {
  public:
    typedef SmoothMonitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Monitor BaseType;

    SmoothMonitor(Size t, NodeId sampledNodeId) : BaseType(t, sampledNodeId) {};
    virtual ~SmoothMonitor() {};

    void SetWeights(const ValArray & weights) { checkSwapped(); weights_.assign(weights.begin(), weights.end()); }
    void SetNodeValues(NodeId nodeId, const Monitor & filterMonitor) { particleValuesMap_[nodeId].assign(filterMonitor.GetNodeValues(nodeId).begin(), filterMonitor.GetNodeValues(nodeId).end()); };
  };

}

#endif /* BIIPS_MONITOR_HPP_ */
