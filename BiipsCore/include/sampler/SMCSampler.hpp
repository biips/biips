//                                               -*- C++ -*-
/*! \file SMCSampler.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from SMCTC smc::sampler class
 */

#ifndef BIIPS_SMCSAMPLER_HPP_
#define BIIPS_SMCSAMPLER_HPP_

#include "NodeSampler.hpp"
#include "Particle.hpp"


namespace Biips
{
  ///Specifiers for various resampling algorithms:
  enum ResampleType { SMC_RESAMPLE_MULTINOMIAL = 0,
    SMC_RESAMPLE_RESIDUAL,
    SMC_RESAMPLE_STRATIFIED,
    SMC_RESAMPLE_SYSTEMATIC };

  class Graph;

  class NodeValuesMap;

  class ScalarAccumulator;
  class DiscreteScalarAccumulator;
  class ElementAccumulator;

  class Monitor;

  class SMCSampler
  {
  public:
    typedef SMCSampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    //---------- Member data ----------
    ///Number of particles in the system.
    Size nParticles_;
    ///The current evolution time of the system.
    Size t_;
    const Graph * pGraph_;
    Rng * pRng_;

    ///The resampling mode which is to be employed.
    ResampleType resampleMode_;
    ///The effective sample size at which resampling should be used.
    Scalar resampleThreshold_;
    ///Structure used internally for resampling.
    Types<Scalar>::Array rsWeights_;
    ///Structure used internally for resampling.
    Types<Size>::Array rsCount_;
    ///Structure used internally for resampling.
    Types<Size>::Array rsIndices_;

    Types<Types<NodeId>::Array>::Iterator iterNodeId_;
    Types<NodeSampler::Ptr>::Iterator iterNodeSampler_;
    Flags sampledFlagsBefore_;
    Flags sampledFlagsAfter_;

    Types<Types<NodeId>::Array>::Array nodeIdSequence_;
    Types<NodeSampler::Ptr>::Array nodeSamplerSequence_;

    Types<Particle>::Array particles_;

    ///A flag which tracks whether the ensemble was resampled during this iteration
    Bool resampled_;

    Bool initialized_;
    Scalar sumOfWeights_;
    Scalar ess_;
    Scalar logNormConst_;

    //---------- Member functions ----------
    void buildNodeIdSequence();
    void buildNodeSamplers();
    Particle initParticle(Rng * pRng);
    void moveParticle(long lTime, Particle & lastParticle, Rng * pRng);
    void resample(ResampleType rsMode);

    // Forbid copying
    SMCSampler(const SMCSampler & from);
    SMCSampler & operator= (const SMCSampler & rhs);

  public:
    static std::list<std::pair<NodeSamplerFactory::Ptr, Bool> > & NodeSamplerFactories();

    SMCSampler(Size nbParticles, Graph * pGraph, Rng * pRng);
    virtual ~SMCSampler() {}

    Size NParticles() const { return nParticles_; };
    Bool IsInitialized() const { return initialized_; };
    Bool IsSampling() const { return iterNodeId_ != nodeIdSequence_.begin(); };
    Bool AtEnd() const { return iterNodeId_ == nodeIdSequence_.end(); };
    Size Time() const { return t_; }
    Size NIterations() const { return nodeSamplerSequence_.size(); };
    Scalar Ess() const { return ess_; }
    Scalar LogNormConst() const { return logNormConst_; }
    Bool Resampled() const { return resampled_; }
    const Types<NodeId>::Array & NextSampledNodes() { return *iterNodeId_; };

    Types<std::pair<NodeId, String> >::Array GetSamplersSequence() const;
    // TODO remove from the class
    void PrintSamplersSequence(std::ostream & os = std::cout) const;
    void PrintSamplerState(std::ostream & os = std::cout) const;

    void SetResampleParams(ResampleType rsMode, Scalar threshold);
    void Initialize();
    void Iterate();

    void Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc) const;
//    template<typename Features>
//    void Accumulate(NodeId nodeId, VectorAccumulator<Features> & featuresAcc) const;

    void SetMonitorWeights(Monitor & monitor);
    void SetMonitorNodeValues(NodeId nodeId, Monitor & monitor);
  };

} /* namespace Biips */

#endif /* BIIPS_SMCSAMPLER_HPP_ */
