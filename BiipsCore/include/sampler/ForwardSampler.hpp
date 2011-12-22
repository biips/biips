//                                               -*- C++ -*-
/*! \file ForwardSampler.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from SMCTC smc::sampler class
 */

#ifndef BIIPS_FORWARDSAMPLER_HPP_
#define BIIPS_FORWARDSAMPLER_HPP_

#include "NodeSampler.hpp"
#include "Particle.hpp"
#include "Resampler.hpp"

namespace Biips
{
  class Graph;

  class ScalarAccumulator;
  class DiscreteScalarAccumulator;
  class ElementAccumulator;

  class FilterMonitor;

  class ForwardSampler
  {
  public:
    typedef ForwardSampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    const Graph & graph_;
    ///Number of particles
    Size nParticles_;
    ///The current iteration
    Size iter_;
    Rng::Ptr pRng_;
    Resampler::Ptr pResampler_;

    ///The ESS threshold under which resampling will be done.
    Scalar resampleThreshold_;

    Types<Types<NodeId>::Array>::Iterator iterNodeId_;
    Types<Types<NodeId>::Array>::Iterator iterObsNodes_;
    Types<NodeSampler::Ptr>::Iterator iterNodeSampler_;
    Flags sampledFlagsBefore_;
    Flags sampledFlagsAfter_;

    Types<Types<NodeId>::Array>::Array nodeIdSequence_;
    Types<Types<NodeId>::Array>::Array obsNodeIdSequence_;
    Types<NodeSampler::Ptr>::Array nodeSamplerSequence_;

    Types<Particle>::Array particles_;

    Types<Size>::Array nodeIterations_;

    Types<Int>::Array nodeLocks_;

    Bool resampled_;

    Bool built_;
    Bool initialized_;
    Scalar sumOfWeights_;
    Scalar ess_;
    Scalar logNormConst_;

    void initLocks();
    void unlockSampledParents();
    void buildNodeIdSequence();
    void buildNodeSamplers();
    void setResampleParams(const String & rsType, Scalar threshold);
    void mutateParticle(Particle & lastParticle);
    Scalar rescaleWeights();
    Scalar sumOfWeightsAndEss();

    // Forbid copying
    ForwardSampler(const ForwardSampler & from);
    ForwardSampler & operator=(const ForwardSampler & rhs);

    static ResamplerTable & resamplerTable()
    {
      static ResamplerTable tab;
      return tab;
    }

  public:
    static std::list<std::pair<NodeSamplerFactory::Ptr, Bool> >
    & NodeSamplerFactories();

    explicit ForwardSampler(const Graph & graph);
    virtual ~ForwardSampler()
    {
    }

    Size NParticles() const;
    Bool AtEnd() const
    {
      return iterNodeId_ + 1 == nodeIdSequence_.end();
    }
    Size Iteration() const
    {
      return iter_;
    }
    Size NIterations() const
    {
      return nodeSamplerSequence_.size();
    }
    Scalar ESS() const
    {
      return ess_;
    }
    Scalar LogNormConst() const
    {
      return logNormConst_;
    }
    const Types<NodeId>::Array & SampledNodes()
    {
      return *iterNodeId_;
    }

    Scalar GetNodeESS(NodeId nodeId) const;

    Size GetNodeSamplingIteration(NodeId nodeId) const;
    const Types<Size>::Array & GetNodeSamplingIterations() const;
    Types<std::pair<NodeId, String> >::Array GetSamplersSequence() const;

    void Build()
    {
      buildNodeIdSequence();
      buildNodeSamplers();
      built_ = true;
    }
    Bool Built() const
    {
      return built_;
    }

    void LockNode(NodeId id)
    {
      nodeLocks_[id]++;
    }
    void UnlockNode(NodeId id)
    {
      nodeLocks_[id]--;
    }
    void UnlockAllNodes();

    void Initialize(Size nbParticles,
                    const Rng::Ptr & pRng,
                    const String & rsType = "stratified",
                    Scalar threshold = 0.5);
    void Iterate();

    Bool Initialized() const
    {
      return initialized_;
    }
    Bool Resampled() const
    {
      return resampled_;
    }

    void Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId,
                    DiscreteScalarAccumulator & featuresAcc,
                    Size n = 0) const;
    void Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc) const;

    void InitMonitor(FilterMonitor & monitor) const;
    void MonitorNode(NodeId nodeId, FilterMonitor & monitor) const;

    void ReleaseNodes();
  };

  void printSamplerState(const ForwardSampler & sampler, std::ostream & os);

} /* namespace Biips */

#endif /* BIIPS_FORWARDSAMPLER_HPP_ */
