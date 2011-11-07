//                                               -*- C++ -*-
/*! \file ForwardSampler.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from SMCTC smc::sampler class
 */

#include "sampler/ForwardSampler.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodeSampler.hpp"
#include "graph/ConstantNode.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "sampler/Accumulator.hpp"
#include "model/Monitor.hpp"

namespace Biips
{

  std::list<std::pair<NodeSamplerFactory::Ptr, Bool> > & ForwardSampler::NodeSamplerFactories()
  {
    static std::list<std::pair<NodeSamplerFactory::Ptr, Bool> > ans;
    return ans;
  }


  void ForwardSampler::buildNodeSamplers()
  {
    nodeSamplerSequence_.resize(nodeIdSequence_.size());

    std::list<std::pair<NodeSamplerFactory::Ptr, Bool> >::const_iterator it_sampler_factory
      = NodeSamplerFactories().begin();

    Types<Types<NodeId>::Array>::Iterator it_node_id;
    Types<NodeSampler::Ptr>::Iterator it_node_sampler;

    for(; it_sampler_factory != NodeSamplerFactories().end();
        ++it_sampler_factory)
    {
      if(!it_sampler_factory->second)
        continue;

      it_node_id = nodeIdSequence_.begin();
      it_node_sampler = nodeSamplerSequence_.begin();
      for(; it_node_id != nodeIdSequence_.end();
          ++it_node_id, ++it_node_sampler)
      {
        if ( !(*it_node_sampler) )
          it_sampler_factory->first->Create(graph_, it_node_id->front(), *it_node_sampler);
      }
    }

    // assign default prior NodeSampler to all non assigned nodes
    it_node_id = nodeIdSequence_.begin();
    it_node_sampler = nodeSamplerSequence_.begin();
    for(; it_node_id != nodeIdSequence_.end();
        ++it_node_id, ++it_node_sampler)
    {
      if ( !(*it_node_sampler) )
        NodeSamplerFactory::Instance()->Create(graph_, it_node_id->front(), *it_node_sampler);
    }
  }


  ForwardSampler::ForwardSampler(Size nbParticles, Graph & graph, const Rng::Ptr & pRng)
  : nParticles_(nbParticles), graph_(graph), pRng_(pRng),
    resampleThreshold_(0.5 * nbParticles),
    sampledFlagsBefore_(graph.GetSize()),
    nodeIterations_(graph.GetSize(), BIIPS_SIZENA),
    built_(false), initialized_(false)
  {
    if (!resamplerTable().Contains("stratified"))
      throw LogicError("StratifiedResampler not found in the ResamplerTable.");
    pResampler_ = resamplerTable()["stratified"];
    if (!pResampler_)
      throw LogicError("Resampler::Ptr is Null.");
  }


  Scalar ForwardSampler::GetNodeESS(NodeId nodeId) const
  {
    if (!Initialized())
      throw LogicError("Can not GetNodeESS: sampler not initialized.");

    Size iter = GetNodeSamplingIteration(nodeId);

    if (iter > iter_)
      throw LogicError("Can not get node ess: node has not been sampled.");

    if (std::find(iterNodeId_->begin(), iterNodeId_->end(), nodeId) != iterNodeId_->end())
      return ess_;

    std::map<ValArray*, Types<Size>::Array> indices_table;
    for (Size i=0; i<nParticles_; ++i)
      indices_table[particles_[i].GetValue()[nodeId].get()].push_back(i);

    typedef long double LongScalar;
    LongScalar sum_sq = 0.0;
    LongScalar sum_temp;
    for (std::map<ValArray*, Types<Size>::Array>::const_iterator it=indices_table.begin();
        it!=indices_table.end(); ++it)
    {
      sum_temp = 0.0;
      for (Size i=0; i<it->second.size(); ++i)
        sum_temp += particles_[it->second[i]].Weight();

      sum_sq += std::pow(sum_temp, 2);
    }
    if (sum_sq == 0.0)
      throw NumericalError("Failure to calculate node ESS: sum of squared weights is null.");

    return std::exp(-std::log(sum_sq) + 2.0*std::log(LongScalar(sumOfWeights_)));
  }
  
  
  class BuildNodeIdSequenceVisitor : public ConstNodeVisitor
  {
  protected:
    typedef BuildNodeIdSequenceVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph & graph_;
    Types<Types<NodeId>::Array>::Array & nodeIdSequence_;
    GraphTypes::IterationsMap nodeIterationsMap_;

    virtual void visit(const ConstantNode & node) {}

    virtual void visit(const StochasticNode & node)
    {
      if ( !graph_.GetObserved()[nodeId_] )
      {
        // push a new array whose first element is the stochastic node
        nodeIdSequence_.push_back(Types<NodeId>::Array(1, nodeId_));
        nodeIterationsMap_[nodeId_] = nodeIdSequence_.size()-1;
      }
    }

    virtual void visit(const LogicalNode & node)
    {
      if ( !graph_.GetObserved()[nodeId_] )
      {
        if (nodeIdSequence_.empty())
          throw LogicError("BuildNodeIdSequenceVisitor can not push LogicalNode in node id sequence: unobserved LogicalNode can not appear before any StochasticNode in the topological order.");

        // push all the logical children back in the last array
        nodeIdSequence_.back().push_back(nodeId_);
        nodeIterationsMap_[nodeId_] = nodeIdSequence_.size()-1;
      }
    }

  public:

    BuildNodeIdSequenceVisitor(const Graph & graph,
        Types<Types<NodeId>::Array>::Array & nodeIdSequence,
        Types<Size>::Array & nodeIterations)
    : graph_(graph), nodeIdSequence_(nodeIdSequence),
      nodeIterationsMap_(boost::make_iterator_property_map(nodeIterations.begin(), boost::identity_property_map())) {};
  };


  void ForwardSampler::buildNodeIdSequence()
  {
    // this will build an array of NodeId arrays whose first
    // element are the unobserved stochastic nodes
    // followed by their logical children
    // in topological order
    BuildNodeIdSequenceVisitor build_node_id_sequence_vis(graph_, nodeIdSequence_, nodeIterations_);
    graph_.VisitGraph(build_node_id_sequence_vis);
  }


  Types<std::pair<NodeId, String> >::Array ForwardSampler::GetSamplersSequence() const
  {
    Types<std::pair<NodeId, String> >::Array ans;
    for (Size k = 0; k<nodeIdSequence_.size(); ++k)
      ans.push_back(std::make_pair(nodeIdSequence_[k].front(), nodeSamplerSequence_[k]->Name()));

    return ans;
  }


  Size ForwardSampler::GetNodeSamplingIteration(NodeId nodeId) const
  {
    if (!built_)
      throw LogicError("Can't get node sampling iteration: sampler not built!");

    if (graph_.GetObserved()[nodeId])
      throw LogicError("Observed nodes can't have sampling iteration!");

    if (nodeIterations_[nodeId] == BIIPS_SIZENA)
      throw LogicError("Node have no sampling iteration yet!");

    return nodeIterations_[nodeId];
  }
  
  
  const Types<Size>::Array & ForwardSampler::GetNodeSamplingIterations() const
  {
    if (!built_)
      throw LogicError("Can't get node sampling iterations: sampler not built!");

    return nodeIterations_;
  }
  
  
  void ForwardSampler::SetResampleParams(const String & rsType, Scalar threshold)
  {
    if (!resamplerTable().Contains(rsType))
      throw LogicError(rsType+" Resampler not found in the ResamplerTable.");
    pResampler_ = resamplerTable()[rsType];
    if (!pResampler_)
      throw LogicError("Resampler::Ptr is Null.");

    resampleThreshold_ = threshold < 1.0 ? threshold * nParticles_ : threshold;
  }


  void ForwardSampler::mutateParticle(Particle & lastParticle)
  {
    // sample current stochastic node
    sampledFlagsAfter_ = sampledFlagsBefore_;
    (*iterNodeSampler_)->SetMembers(lastParticle.Value(), sampledFlagsAfter_, pRng_);
    (*iterNodeSampler_)->Sample(iterNodeId_->front());

    // update particle log weight
    Scalar log_incr_weight = (*iterNodeSampler_)->LogIncrementalWeight();
    lastParticle.AddToLogWeight(log_incr_weight);

    // compute all logical children
    Types<NodeId>::Iterator it_logical_children = iterNodeId_->begin()+1;
    while(it_logical_children != iterNodeId_->end())
    {
      (*iterNodeSampler_)->Sample(*it_logical_children);
      ++it_logical_children;
    }
  }

  Scalar ForwardSampler::rescaleWeights()
  {
    //Rescale the weights to sensible values...
    //in order to avoid numerical instability
    //but they will still not sum to 1
    Scalar max_log_weight = particles_[0].LogWeight();
    for(Size i = 1; i < nParticles_; ++i)
      max_log_weight = std::max(max_log_weight, particles_[i].LogWeight());

    if (!isFinite(max_log_weight))
      return 0.0;

    for(Size i = 0; i < nParticles_; ++i)
      particles_[i].AddToLogWeight(-max_log_weight);

    return max_log_weight;
  }


  Scalar ForwardSampler::sumOfWeightsAndEss()
  {
    typedef long double LongScalar;
    LongScalar sum = 0.0;
    for(Size i = 0; i < nParticles_; ++i)
    {
      sum += std::exp(LongScalar(particles_[i].LogWeight()));
    }
    if(sum == 0.0)
      throw NumericalError("Failure to calculate ESS: sum of weights is null.");

    LongScalar sum_sq = 0.0;
    for(Size i = 0; i < nParticles_; ++i)
      sum_sq += std::exp(2.0*LongScalar(particles_[i].LogWeight()));
    if (sum_sq == 0.0)
      throw NumericalError("Failure to calculate ESS: sum of squared weights is null.");

    ess_ = std::exp(-std::log(sum_sq) + 2.0*std::log(sum));

    return sum;
  }


  void ForwardSampler::Initialize()
  {
    if (!built_)
      throw LogicError("Can not initialize ForwardSampler: not built.");

    iter_ = 0;

    iterNodeId_ = nodeIdSequence_.begin();
    iterNodeSampler_ = nodeSamplerSequence_.begin();

    for(NodeId id = 0; id < graph_.GetSize(); ++id)
      sampledFlagsBefore_[id] = graph_.GetObserved()[id];

    //Initialize the particle set.
    NodeValues init_node_values(graph_.GetSize());
    for(NodeId id = 0; id < graph_.GetSize(); ++id)
      init_node_values[id] = graph_.GetValues()[id];
    particles_.assign(nParticles_, Particle(init_node_values, 0.0));

    //Move the particle set.
    for(Size i = 0; i < nParticles_; ++i)
      mutateParticle(particles_[i]);

    //Rescale the weights to sensible values....
    Scalar max_weight = rescaleWeights();

    sumOfWeights_ = sumOfWeightsAndEss();
    if (isNan(ess_))
      throw NumericalError(String("Failure to calculate ESS."));

    //Check if the ESS is below some reasonable threshold.
    //A mechanism for setting this threshold is required.
    resampled_ = ess_ < resampleThreshold_;

    // increment the normalizing constant
    logNormConst_ = std::log(sumOfWeights_) - std::log(nParticles_) + max_weight;
    if (isNan(logNormConst_))
      throw NumericalError(String("Failure to calculate log normalizing constant."));

    initialized_ = true;
  }


  void ForwardSampler::Iterate()
  {
    if (!initialized_)
      throw LogicError("Can not iterate ForwardSampler: not initialized.");

    if ( AtEnd() )
      throw LogicError("Can not iterate ForwardSampler: the sequence have reached the end.");

    ++iter_;
    ++iterNodeId_;
    ++iterNodeSampler_;

    sampledFlagsBefore_ = sampledFlagsAfter_;

    // Resample if necessary.
    if (resampled_)
      pResampler_->Resample(particles_, sumOfWeights_, *pRng_);

    // Move the particle set.
    for(Size i = 0; i < nParticles_; ++i)
      mutateParticle(particles_[i]);

    // Rescale the weights to sensible values....
    Scalar max_weight = rescaleWeights();

    Scalar sum = sumOfWeightsAndEss();
    if(isNan(ess_))
      throw NumericalError("Failure to calculate ESS.");

    // Check if the ESS is below some reasonable threshold.
    // A mechanism for setting this threshold is required.
    resampled_ = ess_ < resampleThreshold_;

    // Increment the normalizing constant
    logNormConst_ += std::log(sum) - std::log(sumOfWeights_) + max_weight;
    if (isNan(logNormConst_))
      throw NumericalError("Failure to calculate log normalizing constant.");

    sumOfWeights_ = sum;
  }


  void ForwardSampler::Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n) const
  {
    Size iter = GetNodeSamplingIteration(nodeId);
    if (iter > Iteration())
      throw LogicError("Can't Accumulate: node has not been sampled yet!");
  
    featuresAcc.Init();
    for(Size i=0; i < nParticles_; i++)
      featuresAcc.Push((*(particles_[i].GetValue()[nodeId]))[n], particles_[i].Weight());
  }


  void ForwardSampler::Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n) const
  {
    Size iter = GetNodeSamplingIteration(nodeId);
    if (iter > Iteration())
      throw LogicError("Can't Accumulate: node has not been sampled yet!");
  
    featuresAcc.Init();
    for(Size i=0; i < nParticles_; i++)
      featuresAcc.Push((*(particles_[i].GetValue()[nodeId]))[n], particles_[i].Weight());
  }


  void ForwardSampler::Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc) const
  {
    Size iter = GetNodeSamplingIteration(nodeId);
    if (iter > Iteration())
      throw LogicError("Can't Accumulate: node has not been sampled yet!");
  
    featuresAcc.Init(graph_.GetNode(nodeId).DimPtr());
    for(Size i=0; i < nParticles_; i++)
      featuresAcc.Push(particles_[i].GetValue()[nodeId], particles_[i].Weight());
  }


  void ForwardSampler::InitMonitor(FilterMonitor & monitor) const
  {
    monitor.Init(particles_, ess_, sumOfWeights_, resampled_, logNormConst_);
  }


  void ForwardSampler::MonitorNode(NodeId nodeId, FilterMonitor & monitor) const
  {
    Size iter = GetNodeSamplingIteration(nodeId);
    if (iter > Iteration())
      throw LogicError("Can't SetMonitorNodeValues: node has not been sampled yet!");

    monitor.AddNode(nodeId, particles_, iter);

    if (!monitor.HasIterationESS(iter))
      monitor.SetIterationESS(iter, GetNodeESS(nodeId));
  }


  void printSamplerState(const ForwardSampler & sampler, std::ostream & os)
  {
    if (!sampler.Initialized())
      os << "Not initialized" << std::endl;
    else
    {
      Size old_prec = os.precision();
      os.precision(4);
      os << sampler.Iteration()
          << ": ESS = " << sampler.ESS()
          << " (" << std::fixed << sampler.ESS()/sampler.NParticles() << "N)";
      if (!sampler.AtEnd() && sampler.Resampled())
        os << ", Resampled";
      os << std::endl;
      os.precision(old_prec);
    }
  }


} /* namespace Biips */
