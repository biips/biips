/*! 
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

  class Accumulator;
  class DensityAccumulator;
  class QuantileAccumulator;
  class DiscreteAccumulator;
  class ArrayAccumulator;

  class FilterMonitor;

  class SMCIteration
  {
  public:
    typedef SMCIteration SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    Types<NodeId>::Array sampledNodes_;
    Types<NodeId>::Array likeNodes_;
    Types<NodeId>::Array topCondNodes_;
    NodeSampler::Ptr pNodeSampler_;

  public:
    explicit SMCIteration(NodeId stoUnobs, const Types<NodeId>::Array & topCond) :
    sampledNodes_(1, stoUnobs), topCondNodes_(topCond)
    {
    }
    virtual ~SMCIteration()
    {
    }

    // modifiers
    void PushLogicalChild(NodeId child) { sampledNodes_.push_back(child); }
    void PushLikeChild(NodeId like) { likeNodes_.push_back(like); }

    // accessors
    NodeId StoUnobs() const { return sampledNodes_.front(); }
    // sampledNodes_ first element is a stochastic unobserved node.
    // the following elements are logical children

    // sampled nodes at the current iteration (incremental)
    const Types<NodeId>::Array & SampledNodes() const { return sampledNodes_; }
    // top-level conditional nodes at the current iteration (incremental)
    const Types<NodeId>::Array & TopConditionalNodes() const { return topCondNodes_; }
    // likelihood conditional nodes at the current iteration (incremental)
    const Types<NodeId>::Array & LikelihoodNodes() const { return likeNodes_; }

    const NodeSampler::Ptr & NodeSamplerPtr() const { return pNodeSampler_; }

    // accessor/modifier
    NodeSampler::Ptr & NodeSamplerPtr() { return pNodeSampler_; }

  };


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
    Rng * pRng_;
    Resampler::Ptr pResampler_;

    ///The ESS threshold under which resampling will be done.
    Scalar resampleThreshold_;

    Types<Types<SMCIteration>::Array >::Array smcIterations_;

    Flags sampledFlagsBefore_;
    Flags sampledFlagsAfter_;

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
      return NIterations() == 0 || iter_ + 1 == NIterations();
    }
    // iteration starts at 0
    Size Iteration() const
    {
      return iter_;
    }
    Size NIterations() const
    {
      return smcIterations_.size();
    }
    Scalar ESS() const
    {
      return ess_;
    }
    Scalar LogNormConst() const
    {
      return logNormConst_;
    }
    // last sampled nodes at the current iteration (incremental)
    Types<NodeId>::Array LastSampledNodes();
    // all past sampled nodes at the current iteration
    Types<NodeId>::Array SampledNodes();
    // all past conditional nodes at the current iteration
    Types<NodeId>::Array ConditionalNodes();

    Scalar GetNodeESS(NodeId nodeId) const;


    void Build();

//    void Reset();

    Bool Built() const
    {
      return built_;
    }

    Size GetNodeSamplingIteration(NodeId nodeId) const;
    const Types<Size>::Array & GetNodeSamplingIterations() const;
    std::map<NodeId, String> GetNodeSamplersMap() const;

    Types<NodeId>::Array GetStoUnobs(Size iter);
    Types<NodeId>::Array GetLogical(Size iter);
    Types<NodeId>::Array GetLikeChildren(Size iter);

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
                    Rng * pRng,
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

    void Accumulate(NodeId nodeId, Accumulator & featuresAcc, Size n = 0) const;
    void
        Accumulate(NodeId nodeId, DensityAccumulator & featuresAcc, Size n = 0) const;
    void
        Accumulate(NodeId nodeId, QuantileAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, DiscreteAccumulator & featuresAcc, Size n =
        0) const;
    void Accumulate(NodeId nodeId, ArrayAccumulator & featuresAcc) const;

    void InitMonitor(FilterMonitor & monitor) const;
    void MonitorNode(NodeId nodeId, FilterMonitor & monitor) const;

    void ReleaseNodes();
  };

//  void printSamplerState(const ForwardSampler & sampler, std::ostream & os);

} /* namespace Biips */

#endif /* BIIPS_FORWARDSAMPLER_HPP_ */
