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
    ValArray weights_;
    Scalar ess_;
    Scalar sumOfWeights_;
    Types<NodeId>::Array sampledNodes_;
    typedef Types<ValArray::Ptr>::Array ParticleValues;
    std::map<NodeId, ParticleValues> particleValuesMap_;
    std::map<NodeId, Size> nodeIterationMap_;
    std::map<Size, Scalar> iterationEssMap_;

    Bool weightsSet_;
    Bool weightsSwapped_;
    void checkWeights() const;
    void checkSwapped() const;

  public:
    Monitor(Size t, const Types<NodeId>::Array & sampledNodes)
    	: t_(t), sampledNodes_(sampledNodes), weightsSet_(false), weightsSwapped_(false) {}
    virtual ~Monitor() {}

    Bool Contains(NodeId nodeId) const;
    Types<NodeId>::Array GetNodes() const;

    Size NParticles() const { checkWeights(); checkSwapped(); return weights_.size(); }
    Size GetIteration() const { return t_; }
    const Types<NodeId>::Array & GetSampledNodes() const { return sampledNodes_; }
    Scalar GetESS() const { checkWeights(); return ess_; }
    Size GetNodeSamplingIteration(NodeId id) const;
    Bool HasIterationESS(Size iter) const;
    void SetIterationESS(Size iter, Scalar ess);
    Scalar GetNodeESS(NodeId id) const;
    Scalar GetSumOfWeights() const { checkWeights(); return sumOfWeights_; }
    const ValArray & GetUnnormWeights() const { checkWeights(); checkSwapped(); return weights_; }
    ValArray GetWeights() const { checkWeights(); checkSwapped(); return weights_/sumOfWeights_; }
    Bool WeightsSet() const { return weightsSet_; }

    void SwapWeights(ValArray::BaseType & vec) { weights_.swap(vec); weightsSwapped_ = !weightsSwapped_; }

    const ParticleValues & GetNodeValues(NodeId nodeId) const { return particleValuesMap_.at(nodeId); }

    void Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc, const DimArray::Ptr & pDim) const;

    void ClearNode(NodeId nodeId) { particleValuesMap_.erase(nodeId); };
  };


  class FilterMonitor : public Monitor
  {
  public:
    typedef FilterMonitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Monitor BaseType;

  protected:
    Bool resampled_;
    Scalar logNormConst_;

  public:
    FilterMonitor(Size t, const Types<NodeId>::Array & sampledNodes) : BaseType(t, sampledNodes) {};
    virtual ~FilterMonitor() {};

    void Init(const Types<Particle>::Array & particles, Scalar ess, Scalar sumOfWeights, Bool resampled, Scalar logNormConst);
    void AddNode(NodeId nodeId, const Types<Particle>::Array & particles, Size iter);

    Bool GetResampled() const { checkWeights(); return resampled_; }
    Scalar GetLogNormConst() const { checkWeights(); return logNormConst_; }
  };


  class SmoothMonitor : public Monitor
  {
  public:
    typedef SmoothMonitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Monitor BaseType;

    SmoothMonitor(Size t, const Types<NodeId>::Array & sampledNodes) : BaseType(t, sampledNodes) {};
    virtual ~SmoothMonitor() {};

    void Init(const ValArray & weights, Scalar ess, Scalar sumOfWeights);
    void AddNode(NodeId nodeId, const Monitor & filterMonitor);
  };

}

#endif /* BIIPS_MONITOR_HPP_ */
