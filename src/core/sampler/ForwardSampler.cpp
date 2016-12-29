/*! \file ForwardSampler.cpp
 * COPY: Adapted from SMCTC smc::sampler class
 */

#include "sampler/ForwardSampler.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodeSampler.hpp"
#include "graph/ConstantNode.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "common/Accumulator.hpp"
#include "common/ArrayAccumulator.hpp"
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
    // list of node sampler factories
    std::list<std::pair<NodeSamplerFactory::Ptr, Bool> >::const_iterator
    it_sampler_factory = NodeSamplerFactories().begin();

    Types<SMCIteration>::Iterator it_smc_iter;

    // loop over node sampler factories
    // in priority order
    for (; it_sampler_factory != NodeSamplerFactories().end(); ++it_sampler_factory)
    {
      if (!it_sampler_factory->second)
        continue;

      // loop over iterations
      for (Size i=0; i<smcIterations_.size(); ++i)
      {
        // loop over nodes in iteration i
        it_smc_iter = smcIterations_.at(i).begin();
        for (; it_smc_iter != smcIterations_.at(i).end(); ++it_smc_iter)
        {
          // if null, i.e not already assigned a sampler
          if (!it_smc_iter->NodeSamplerPtr())
            it_sampler_factory->first->Create(graph_,
                                              it_smc_iter->StoUnobs(),
                                              it_smc_iter->NodeSamplerPtr());
          // if the current node sampler factory conditions are met
          // Create method assigns the node sampler to it_smc_iter->NodeSamplerPtr()
          // otherwise does not change it (null)
        }
      }
    }

    // assign default prior NodeSampler to all non assigned nodes
    // loop over iterations
    for (Size i=0; i<smcIterations_.size(); ++i)
    {
      it_smc_iter = smcIterations_.at(i).begin();
      for (; it_smc_iter != smcIterations_.at(i).end(); ++it_smc_iter)
      {
        // if null, i.e not already assigned a sampler
        if (!it_smc_iter->NodeSamplerPtr())
          NodeSamplerFactory::Instance()->Create(graph_,
                                                 it_smc_iter->StoUnobs(),
                                                 it_smc_iter->NodeSamplerPtr());
      }
    }
  }

  void ForwardSampler::initLocks()
  {
    std::fill(nodeLocks_.begin(), nodeLocks_.end(), 0);
    Types<NodeId>::ConstIterator it_nodes, it_nodes_end;
    boost::tie(it_nodes, it_nodes_end) = graph_.GetSortedNodes();
    for (; it_nodes != it_nodes_end; ++it_nodes)
    {
      if (graph_.GetObserved()[*it_nodes])
      {
        // we don't want to release observed nodes
        nodeLocks_[*it_nodes] = -1;

        // we need to lock parents of stochastic observed nodes to calculate likelihood
        // but we don't need to lock parents of deterministic observed nodes
        if (graph_.GetNode(*it_nodes).GetType() != STOCHASTIC)
          continue;
      }

      GraphTypes::ParentIterator it_parents, it_parents_end;
      boost::tie(it_parents, it_parents_end) = graph_.GetParents(*it_nodes);
      for (; it_parents != it_parents_end; ++it_parents)
        if (!graph_.GetObserved()[*it_parents])
          LockNode(*it_parents);
    }
  }

  void ForwardSampler::unlockSampledParents()
  {
    for (Size i=0; i<smcIterations_.at(iter_).size(); ++i)
    {
      const Types<NodeId>::Array & sampled_nodes = smcIterations_.at(iter_).at(i).SampledNodes();
      Types<NodeId>::ConstIterator it_sampled_nodes = sampled_nodes.begin();
      for (; it_sampled_nodes != sampled_nodes.end(); ++it_sampled_nodes)
      {
        GraphTypes::ParentIterator it_parents, it_parents_end;
        boost::tie(it_parents, it_parents_end)
        = graph_.GetParents(*it_sampled_nodes);
        for (; it_parents != it_parents_end; ++it_parents)
          UnlockNode(*it_parents);
      }
    }
  }

  ForwardSampler::ForwardSampler(const Graph & graph) :
        graph_(graph), nParticles_(1), resampleThreshold_(BIIPS_POSINF),
        sampledFlagsBefore_(graph.GetSize()), sampledFlagsAfter_(graph.GetSize()),
        nodeIterations_(graph.GetSize(), BIIPS_SIZENA),
        nodeLocks_(graph.GetSize(), 0), built_(false), initialized_(false)
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

    if (graph_.GetObserved()[nodeId])
      throw LogicError("Can not get node ess: node is observed.");

    Size iter = GetNodeSamplingIteration(nodeId);

    if (iter > iter_)
      throw LogicError("Can not get node ess: node has not been sampled.");

    // TODO check this
    if (iter==iter_)
      return ess_;
//    for (Size i=0; i<smcIterations_.at(iter_).size(); ++i)
//    {
//      if (smcIterations_.at(iter_).at(i).SamplesNode(nodeId))
//        return ess_;
//    }

    // We store the particle indices of unique values
    // in a map indexed by the value pointers
    std::map<ValArray*, Types<Size>::Array> indices_table;
    for (Size i = 0; i < nParticles_; ++i)
      indices_table[particles_[i].GetValue()[nodeId].get()].push_back(i);

    typedef long double LongScalar;
    LongScalar sum_sq = 0.0;
    LongScalar sum_temp;
    for (std::map<ValArray*, Types<Size>::Array>::const_iterator it =
        indices_table.begin(); it != indices_table.end(); ++it)
    {
      sum_temp = 0.0;
      for (Size i = 0; i < it->second.size(); ++i)
        sum_temp += particles_[it->second[i]].Weight();

      sum_sq += std::pow(sum_temp, 2);
    }
    if (sum_sq == 0.0)
      throw NumericalError("Failure to calculate node ESS: sum of squared weights is null.");

    return std::exp(-std::log(sum_sq) + 2.0
                    * std::log(LongScalar(sumOfWeights_)));
  }

  Types<NodeId>::Array ForwardSampler::LastSampledNodes()
  {
    Types<NodeId>::Array ans;
    for (Size i=0; i<smcIterations_.at(iter_).size(); ++i)
    {
      const Types<NodeId>::Array & sampled = smcIterations_.at(iter_).at(i).SampledNodes();
      ans.insert(ans.end(), sampled.begin(), sampled.end());
    }
    return ans;
  }

  Types<NodeId>::Array ForwardSampler::SampledNodes()
  {
    Types<NodeId>::Array ans;
    for (Size iter=0; iter<=iter_; ++iter) // iter=0,...,iter_
    {
      for (Size i=0; i<smcIterations_.at(iter).size(); ++i)
      {
        const Types<NodeId>::Array & sampled = smcIterations_.at(iter).at(i).SampledNodes();
        ans.insert(ans.end(), sampled.begin(), sampled.end());
      }
    }
    return ans;
  }

  Types<NodeId>::Array ForwardSampler::ConditionalNodes()
  {
    Types<NodeId>::Array ans;
    for (Size iter=0; iter<=iter_; ++iter) // iter=0,...,iter_
    {
      for (Size i=0; i<smcIterations_.at(iter).size(); ++i)
      {
        const Types<NodeId>::Array & top = smcIterations_.at(iter).at(i).TopConditionalNodes();
        ans.insert(ans.end(), top.begin(), top.end());
        const Types<NodeId>::Array & like = smcIterations_.at(iter).at(i).LikelihoodNodes();
        ans.insert(ans.end(), like.begin(), like.end());
      }
    }
    return ans;
  }

  class BuildIterationsVisitor: public ConstNodeVisitor
  {
  protected:
    typedef BuildIterationsVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph & graph_;
    Types<Types<SMCIteration>::Array >::Array & smcIterations_;
    Types<Size>::Array & nodeIterationsMap_;
    Types<NodeId>::Array topCondNodes_;

    virtual void visit(const ConstantNode & node)
    {
    }

    virtual void visit(const StochasticNode & node)
    {
      if (graph_.GetObserved()[nodeId_])
      {
        // determine if the observed stochastic node has any unobserved parent
        Bool has_unobs_parent = false;
        if (!smcIterations_.empty())
        {
          GraphTypes::ParentIterator it_parents, it_parents_end;
          boost::tie(it_parents, it_parents_end) = graph_.GetParents(nodeId_);
          for (; it_parents!=it_parents_end; ++it_parents) {
            if (!graph_.GetObserved()[*it_parents]) {
              has_unobs_parent = true;
              break;
            }
          }
        }

        if (has_unobs_parent) {
          // it is a likelihood conditional node
          smcIterations_.back().back().PushLikeChild(nodeId_);
        }
        else {
          // it is a top-level conditional node
          topCondNodes_.push_back(nodeId_);
        }
        return;
      }

//      // determine if the unobserved stochastic node has any stochastic children
//      Bool has_sto_children = false;
//      GraphTypes::ChildIterator it_chilren, it_chilren_end;
//      boost::tie(it_chilren, it_chilren_end) = graph_.GetChildren(nodeId_);
//      for (; it_chilren!=it_chilren_end; ++it_chilren) {
//        if (graph_.GetNode(*it_chilren).GetType() == STOCHASTIC) {
//          has_sto_children = true;
//          break;
//        }
//      }
//
//      // keep the same iteration if the current node has no stochastic children
//      if (!has_sto_children) {
//        smcIterations_.back().push_back(SMCIteration(nodeId_, topCondNodes_));
//        topCondNodes_.clear();
//      }

      // keep the same iteration if the last node has no likelihood children.
      if (!smcIterations_.empty() && smcIterations_.back().back().LikelihoodNodes().empty()) {
        smcIterations_.back().push_back(SMCIteration(nodeId_, topCondNodes_));
        topCondNodes_.clear();
      }
      // otherwise add new iteration
      else {
        smcIterations_.push_back(Types<SMCIteration>::Array(1, SMCIteration(nodeId_, topCondNodes_)));
        topCondNodes_.clear();
      }

      nodeIterationsMap_[nodeId_] = smcIterations_.size() - 1;
    }

    virtual void visit(const LogicalNode & node)
    {
      if (graph_.GetObserved()[nodeId_])
        return;

      if (smcIterations_.empty())
        throw LogicError("BuildNodeIdSequenceVisitor can not push LogicalNode in empty sequence.");

      // push all the logical children in the last iteration
      smcIterations_.back().back().PushLogicalChild(nodeId_);
      nodeIterationsMap_[nodeId_] = smcIterations_.size() - 1;
    }

  public:

    BuildIterationsVisitor(const Graph & graph,
                           Types<Types<SMCIteration>::Array >::Array & smcIterations,
                           Types<Size>::Array & nodeIterationsMap) :
                             graph_(graph), smcIterations_(smcIterations),
                             nodeIterationsMap_(nodeIterationsMap)
  {
  }
  };

  void ForwardSampler::buildNodeIdSequence()
  {
    BuildIterationsVisitor build_iterations_vis(graph_,
                                                smcIterations_,
                                                nodeIterations_);
    graph_.VisitGraph(build_iterations_vis);
  }

  std::map<NodeId, String> ForwardSampler::GetNodeSamplersMap() const
  {
    std::map<NodeId, String> ans;
    for (Size k = 0; k < smcIterations_.size(); ++k) {
      for (Size i=0; i<smcIterations_.at(k).size(); ++i)
        ans[smcIterations_.at(k).at(i).StoUnobs()] = smcIterations_.at(k).at(i).NodeSamplerPtr()->Name();
    }

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

  void ForwardSampler::Build()
  {
    buildNodeIdSequence();
    buildNodeSamplers();
    built_ = true;
  }

  //  void ForwardSampler::Reset()
  //  {
  //    nodeLocks_.assign(graph_.GetSize(), 0);
  //    nodeIdSequence_.clear();
  //    obsNodeIdSequence_.clear();
  //    nodeIterations_.assign(graph_.GetSize(), BIIPS_SIZENA);
  //    nodeSamplerSequence_.clear();
  //    built_ = false;
  //    initialized_ = false;
  //  }

  void ForwardSampler::mutateParticle(Particle & lastParticle)
  {
    // sample current stochastic node
    std::copy(sampledFlagsBefore_.begin(), sampledFlagsBefore_.end(),
              sampledFlagsAfter_.begin());

    Types<SMCIteration>::Array & smc_iter = smcIterations_.at(iter_);

    for (Size i=0; i<smc_iter.size(); ++i)
    {
      smc_iter.at(i).NodeSamplerPtr()->SetMembers(lastParticle.Value(),
                                      sampledFlagsAfter_,
                                      pRng_);
      smc_iter.at(i).NodeSamplerPtr()->Sample(smc_iter.at(i).StoUnobs());

      // compute all children that are logical
      // TODO only update nodes which have a monitored child
      // TODO update also children that are stochastic with no children
      Types<NodeId>::ConstIterator it_children = smc_iter.at(i).SampledNodes().begin()+1;
      while (it_children != smc_iter.at(i).SampledNodes().end())
      {
        smc_iter.at(i).NodeSamplerPtr()->Sample(*it_children);
        ++it_children;
      }
    }
    // update particle log weight
    // only at the last smc_iter which has observed likelihood children
    Scalar log_incr_weight = smc_iter.back().NodeSamplerPtr()->LogIncrementalWeight();
    lastParticle.AddToLogWeight(log_incr_weight);
  }

  Scalar ForwardSampler::rescaleWeights()
  {
    //Rescale the weights to sensible values...
    //in order to avoid numerical instability
    //but they will still not sum to 1
    Scalar max_log_weight = particles_[0].LogWeight();
    for (Size i = 1; i < nParticles_; ++i)
      max_log_weight = std::max(max_log_weight, particles_[i].LogWeight());

    if (!isFinite(max_log_weight))
      return 0.0;

    for (Size i = 0; i < nParticles_; ++i)
      particles_[i].AddToLogWeight(-max_log_weight);

    return max_log_weight;
  }

  Scalar ForwardSampler::sumOfWeightsAndEss()
  {
    typedef long double LongScalar;
    LongScalar sum = 0.0;
    for (Size i = 0; i < nParticles_; ++i)
    {
      sum += std::exp(LongScalar(particles_[i].LogWeight()));
    }
    if (sum == 0.0)
      throw NumericalError("Failure to calculate ESS: sum of weights is null.");

    LongScalar sum_sq = 0.0;
    for (Size i = 0; i < nParticles_; ++i)
      sum_sq += std::exp(2.0 * LongScalar(particles_[i].LogWeight()));
    if (sum_sq == 0.0)
      throw NumericalError("Failure to calculate ESS: sum of squared weights is null.");

    ess_ = std::exp(-std::log(sum_sq) + 2.0 * std::log(sum));

    return sum;
  }

  void ForwardSampler::setResampleParams(const String & rsType,
                                         Scalar threshold)
  {
    if (!resamplerTable().Contains(rsType))
      throw LogicError(rsType + " Resampler not found in the ResamplerTable.");
    pResampler_ = resamplerTable()[rsType];
    if (!pResampler_)
      throw LogicError("Resampler::Ptr is Null.");

    resampleThreshold_ = threshold <= 1.0 ? threshold * nParticles_ : threshold;
  }

  void ForwardSampler::Initialize(Size nbParticles,
                                  Rng * pRng,
                                  const String & rsType,
                                  Scalar threshold)
  {
    if (!built_)
      throw LogicError("Can not initialize ForwardSampler: not built.");

    if (NIterations() == 0)
      throw LogicError("Can not initialize ForwardSampler: no iterations.");

    nParticles_ = nbParticles;
    pRng_ = pRng;
    setResampleParams(rsType, threshold);

    //locks
    initLocks();

    iter_ = 0;

    for (Size i=0; i<sampledFlagsBefore_.size(); ++i)
      sampledFlagsBefore_.at(i) = graph_.GetObserved()[i];

    //Initialize the particle set.
    NodeValues init_node_values(graph_.GetSize());
    particles_.assign(nParticles_, Particle(init_node_values, 0.0));

    //Move the particle set.
    for (Size i = 0; i < nParticles_; ++i)
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
    logNormConst_ = std::log(sumOfWeights_) - std::log(nParticles_)
    + max_weight;
    if (isNan(logNormConst_))
      throw NumericalError(String("Failure to calculate log normalizing constant."));

    initialized_ = true;

    unlockSampledParents();
  }

  void ForwardSampler::Iterate()
  {
    if (!initialized_)
      throw LogicError("Can not iterate ForwardSampler: not initialized.");

    if (AtEnd())
      throw LogicError("Can not iterate ForwardSampler: the sequence have reached the end.");

    ++iter_;

    sampledFlagsBefore_.swap(sampledFlagsAfter_);

    // Resample if necessary.
    if (resampled_)
      pResampler_->Resample(particles_, sumOfWeights_, *pRng_);

    // Move the particle set.
    for (Size i = 0; i < nParticles_; ++i)
      mutateParticle(particles_[i]);

    // Rescale the weights to sensible values....
    Scalar max_weight = rescaleWeights();

    Scalar sum = sumOfWeightsAndEss();
    if (isNan(ess_))
      throw NumericalError("Failure to calculate ESS.");

    // Check if the ESS is below some reasonable threshold.
    // A mechanism for setting this threshold is required.
    resampled_ = ess_ < resampleThreshold_;

    // Increment the normalizing constant
    logNormConst_ += std::log(sum) - std::log(sumOfWeights_) + max_weight;
    if (isNan(logNormConst_))
      throw NumericalError("Failure to calculate log normalizing constant.");

    sumOfWeights_ = sum;

    unlockSampledParents();
  }

  void ForwardSampler::Accumulate(NodeId nodeId,
                                  Accumulator & featuresAcc,
                                  Size n) const
  {
    Size iter = GetNodeSamplingIteration(nodeId);
    if (iter > Iteration())
      throw LogicError("Can't Accumulate: node has not been sampled yet!");

    featuresAcc.Init();
    for (Size i = 0; i < nParticles_; i++)
      featuresAcc.Push((*(particles_[i].GetValue()[nodeId]))[n],
                       particles_[i].Weight());
  }

  void ForwardSampler::Accumulate(NodeId nodeId,
                                  DensityAccumulator & densAcc,
                                  Size n) const
  {
    Size iter = GetNodeSamplingIteration(nodeId);
    if (iter > Iteration())
      throw LogicError("Can't Accumulate: node has not been sampled yet!");

    densAcc.Init();
    for (Size i = 0; i < nParticles_; i++)
      densAcc.Push((*(particles_[i].GetValue()[nodeId]))[n],
                   particles_[i].Weight());
  }

  void ForwardSampler::Accumulate(NodeId nodeId,
                                  QuantileAccumulator & quantAcc,
                                  Size n) const
  {
    Size iter = GetNodeSamplingIteration(nodeId);
    if (iter > Iteration())
      throw LogicError("Can't Accumulate: node has not been sampled yet!");

    quantAcc.Init();
    for (Size i = 0; i < nParticles_; i++)
      quantAcc.Push((*(particles_[i].GetValue()[nodeId]))[n],
                    particles_[i].Weight());
  }

  void ForwardSampler::Accumulate(NodeId nodeId,
                                  DiscreteAccumulator & featuresAcc,
                                  Size n) const
  {
    Size iter = GetNodeSamplingIteration(nodeId);
    if (iter > Iteration())
      throw LogicError("Can't Accumulate: node has not been sampled yet!");

    featuresAcc.Init();
    for (Size i = 0; i < nParticles_; i++)
      featuresAcc.Push((*(particles_[i].GetValue()[nodeId]))[n],
                       particles_[i].Weight());
  }

  void ForwardSampler::Accumulate(NodeId nodeId, ArrayAccumulator & featuresAcc) const
  {
    Size iter = GetNodeSamplingIteration(nodeId);
    if (iter > Iteration())
      throw LogicError("Can't Accumulate: node has not been sampled yet!");

    featuresAcc.Init(graph_.GetNode(nodeId).DimPtr());
    for (Size i = 0; i < nParticles_; i++)
      featuresAcc.Push(*(particles_[i].GetValue()[nodeId]),
                       particles_[i].Weight());
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

    monitor.AddNode(nodeId, particles_, iter, graph_.GetDiscrete()[nodeId]);

    if (!monitor.HasIterationESS(iter))
      monitor.SetIterationESS(iter, GetNodeESS(nodeId));
  }

  // TODO
//  void printSamplerState(const ForwardSampler & sampler, std::ostream & os)
//  {
//    if (!sampler.Initialized())
//      os << "Not initialized" << std::endl;
//    else
//    {
//      Size old_prec = os.precision();
//      os.precision(4);
//      os << sampler.Iteration() << ": ESS = " << sampler.ESS() << " ("
//          << std::fixed << sampler.ESS() / sampler.NParticles() << "N)";
//      if (!sampler.AtEnd() && sampler.Resampled())
//        os << ", Resampled";
//      os << std::endl;
//      os.precision(old_prec);
//    }
//  }

  Size ForwardSampler::NParticles() const
  {
    if (!initialized_)
      throw LogicError("Can not get number of particles: sampler not initialized.");
    return nParticles_;
  }

  void ForwardSampler::UnlockAllNodes()
  {
    for (Types<Int>::Iterator it_locks = nodeLocks_.begin(); it_locks
    != nodeLocks_.end(); ++it_locks)
    {
      if (*it_locks > 0)
        *it_locks = 0;
    }
  }

  void ForwardSampler::ReleaseNodes()
  {
    Types<NodeId>::ConstIterator it_nodes, it_nodes_end;
    boost::tie(it_nodes, it_nodes_end) = graph_.GetSortedNodes();
    for (; it_nodes != it_nodes_end; ++it_nodes)
    {
      NodeId id = *it_nodes;
      if (nodeLocks_[id] != 0)
        continue;
      for (Size i = 0; i < nParticles_; ++i)
        particles_[i].Value()[id].reset();
      nodeLocks_[id] = -1;
    }
  }

} /* namespace Biips */
