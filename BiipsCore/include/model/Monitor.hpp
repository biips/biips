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
    Size iter_;
    //    ValArray logWeights_;
    ValArray weights_;
    Scalar ess_;
    Scalar sumOfWeights_;
    Types<NodeId>::Array sampledNodes_;
    typedef Types<ValArray::Ptr>::Array ParticleValues;
    std::map<NodeId, ParticleValues> particleValuesMap_;
    std::map<NodeId, Size> nodeIterationMap_;
    std::map<Size, Scalar> iterationEssMap_;
    std::map<NodeId, Bool> nodeDiscreteMap_;

    Bool weightsSet_;
    Bool weightsSwapped_;
    //    Bool logWeightsSwapped_;
    void checkWeightsSet() const;
    void checkWeightsSwapped() const;
    //    void checkLogWeightsSwapped() const;

  public:
    Monitor(Size iter, const Types<NodeId>::Array & sampledNodes) :
      iter_(iter), sampledNodes_(sampledNodes), weightsSet_(false),
          weightsSwapped_(false)/*, logWeightsSwapped_(false)*/
    {
    }
    virtual ~Monitor()
    {
    }

    Bool Contains(NodeId nodeId) const;
    Types<NodeId>::Array GetNodes() const;

    Size NParticles() const
    {
      checkWeightsSet();
      checkWeightsSwapped();
      return weights_.size();
    }
    Size GetIteration() const
    {
      return iter_;
    }
    const Types<NodeId>::Array & GetSampledNodes() const
    {
      return sampledNodes_;
    }
    Scalar GetESS() const
    {
      checkWeightsSet();
      return ess_;
    }
    Size GetNodeSamplingIteration(NodeId id) const;
    Bool HasIterationESS(Size iter) const;
    void SetIterationESS(Size iter, Scalar ess);
    Scalar GetNodeESS(NodeId id) const;
    Bool GetNodeDiscrete(NodeId id) const;
    Scalar GetSumOfWeights() const
    {
      checkWeightsSet();
      return sumOfWeights_;
    }
    //    const ValArray & GetUnnormLogWeights() const
    //    {
    //      checkWeightsSet();
    //      checkLogWeightsSwapped();
    //      return logWeights_;
    //    }
    const ValArray & GetUnnormWeights() const
    {
      checkWeightsSet();
      checkWeightsSwapped();
      return weights_;
    }
    ValArray GetWeights() const
    {
      checkWeightsSet();
      checkWeightsSwapped();
      return weights_ / sumOfWeights_;
    }
    Bool WeightsSet() const
    {
      return weightsSet_;
    }

    //    void SwapLogWeights(ValArray::BaseType & vec)
    //    {
    //      logWeights_.swap(vec);
    //      logWeightsSwapped_ = !logWeightsSwapped_;
    //    }

    void SwapWeights(ValArray::BaseType & vec)
    {
      weights_.swap(vec);
      weightsSwapped_ = !weightsSwapped_;
    }

    const ParticleValues & GetNodeValues(NodeId nodeId) const
    {
      return particleValuesMap_.at(nodeId);
    }

    void
    Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId,
                    DiscreteScalarAccumulator & featuresAcc,
                    Size n = 0) const;
    void Accumulate(NodeId nodeId,
                    ElementAccumulator & featuresAcc,
                    const DimArray::Ptr & pDim) const;

    void ClearNode(NodeId nodeId)
    {
      particleValuesMap_.erase(nodeId);
    }
  };

  class FilterMonitor: public Monitor
  {
  public:
    typedef FilterMonitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Monitor BaseType;

  protected:
    Bool resampled_;
    Scalar logNormConst_;

  public:
    FilterMonitor(Size iter, const Types<NodeId>::Array & sampledNodes) :
      BaseType(iter, sampledNodes)
    {
    }
    virtual ~FilterMonitor()
    {
    }

    void Init(const Types<Particle>::Array & particles,
              Scalar ess,
              Scalar sumOfWeights,
              Bool resampled,
              Scalar logNormConst);
    void AddNode(NodeId nodeId,
                 const Types<Particle>::Array & particles,
                 Size iter,
                 Bool discrete);

    Bool GetResampled() const
    {
      checkWeightsSet();
      return resampled_;
    }
    Scalar GetLogNormConst() const
    {
      checkWeightsSet();
      return logNormConst_;
    }
  };

  class SmoothMonitor: public Monitor
  {
  public:
    typedef SmoothMonitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Monitor BaseType;

    SmoothMonitor(Size iter, const Types<NodeId>::Array & sampledNodes) :
      BaseType(iter, sampledNodes)
    {
    }
    virtual ~SmoothMonitor()
    {
    }

    void Init(const ValArray & weights, Scalar ess, Scalar sumOfWeights);
    void AddNode(NodeId nodeId, const Monitor & filterMonitor);
  };

}

#endif /* BIIPS_MONITOR_HPP_ */
