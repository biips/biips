//                                               -*- C++ -*-
/*! \file SMCSampler.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_SMCSAMPLER_HPP_
#define BIIPS_SMCSAMPLER_HPP_

#include "MoveSet.hpp"
#include "NodeSampler.hpp"


namespace Biips
{


//  ///Specifiers for various resampling algorithms:
//  enum ResampleType { SMC_RESAMPLE_MULTINOMIAL = 0,
//    SMC_RESAMPLE_RESIDUAL,
//    SMC_RESAMPLE_STRATIFIED,
//    SMC_RESAMPLE_SYSTEMATIC };
//
//  ///Storage types for the history of the particle system.
//  enum HistoryType { SMC_HISTORY_NONE = 0,
//    SMC_HISTORY_RAM };


  class Graph;

  class NodeValuesMap;

  class ScalarAccumulator;
  class DiscreteScalarAccumulator;
  class ElementAccumulator;

  class Monitor;


  class SMCSampler : protected smc::sampler<NodeValues>
  {
  public:
    typedef SMCSampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef smc::sampler<NodeValues> BaseType;
//    typedef smc::historyflags HistoryFlags;

    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    //---------- Member data ----------
    static Graph * pGraph_;
    static Types<Types<NodeId>::Array>::Iterator & iterNodeId() { static Types<Types<NodeId>::Array>::Iterator ans; return ans; };
    static Types<NodeSampler::Ptr>::Iterator & iterNodeSampler() { static Types<NodeSampler::Ptr>::Iterator ans; return ans; };
    static Flags & sampledFlagsBefore();
    static Flags & sampledFlagsAfter();

    Types<Types<NodeId>::Array>::Array nodeIdSequence_;
    Types<NodeSampler::Ptr>::Array nodeSamplerSequence_;
//    Biips::History History;
    Biips::MoveSet Moves;
    Rng * pRng_;

    Bool initialized_;
    Scalar sumOfWeights_;
    Scalar ess_;
    Scalar logNormConst_;

    //---------- Member functions ----------
//    static void mutateNode(NodeId nodeId, MutationNodeVisitor::Ptr pMutationVis, NodeValues & particleValue, Flags & sampledFlags, Rng * pRng);
    static Particle fInitialize(Rng * pRng);
    static void fMove(long lTime, Particle & lastParticle, Rng * pRng);

    void buildNodeIdSequence();
    void buildNodeSamplers();

    // Forbid copying
    SMCSampler(const SMCSampler & from);
    SMCSampler & operator= (const SMCSampler & rhs);

  public:
    static std::list<std::pair<NodeSamplerFactory::Ptr, Bool> > & NodeSamplerFactories();

    static const Types<NodeId>::Array & NextSampledNodes() { return *iterNodeId(); };

    Size NParticles() const { return Size(N); };
    Bool IsInitialized() const { return initialized_; };
    Bool IsSampling() const { return iterNodeId() != nodeIdSequence_.begin(); };
    Bool AtEnd() const { return iterNodeId() == nodeIdSequence_.end(); };
    Size NIterations() const { return nodeSamplerSequence_.size(); };
    Scalar Ess() const { return ess_; }
    Scalar LogNormConst() const { return logNormConst_; }
    Bool Resampled() const { return nResampled; }

    Types<std::pair<NodeId, String> >::Array GetSamplersSequence() const;
    // TODO remove from the class
    void PrintSamplersSequence(std::ostream & os = std::cout) const;
    void PrintSamplerState(std::ostream & os = std::cout) const;

    void SetResampleParams(ResampleType rtMode, Scalar threshold) { BaseType::SetResampleParams(rtMode, threshold < 1.0 ? threshold : N); }
    void Initialize();
    void Iterate();

    void Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc) const;
//    template<typename Features>
//    void Accumulate(NodeId nodeId, VectorAccumulator<Features> & featuresAcc) const;

    void MonitorNode(NodeId nodeId, Monitor & monitor);


    //---------- Constructors and Destructors ----------
    SMCSampler(Size nbParticles, Graph * pGraph, Rng * pRng);
    virtual ~SMCSampler() {};
  };




} /* namespace Biips */

#endif /* BIIPS_SMCSAMPLER_HPP_ */
