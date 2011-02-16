//                                               -*- C++ -*-
/*! \file SMCSampler.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/SMCSampler.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodeSampler.hpp"
#include "graph/ConstantNode.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "sampler/Accumulator.hpp"


namespace Biips
{

  Graph * SMCSampler::pGraph_;

  Flags & SMCSampler::sampledFlagsBefore() { static Flags ans(pGraph_->GetSize()); return ans; };
  Flags & SMCSampler::sampledFlagsAfter() { static Flags ans(pGraph_->GetSize()); return ans; };

  void SMCSampler::buildNodeSamplers()
  {
    nodeSamplerSequence_.resize(nodeIdSequence_.size());
    Flags flags_node_sampler_affected_sequence(nodeIdSequence_.size(), false);

    Types<NodeSamplerFactory::Ptr>::ReverseIterator rit_node_sampler_factory_order = nodeSamplerFactoryInvOrder_.rbegin();

    Types<Types<NodeId>::Array>::Iterator it_node_id;
    Types<NodeSampler::Ptr>::Iterator it_node_sampler;
    Flags::iterator it_flags_node_sampler_affected;

    while ( rit_node_sampler_factory_order != nodeSamplerFactoryInvOrder_.rend() )
    {
      it_node_id = nodeIdSequence_.begin();
      it_node_sampler = nodeSamplerSequence_.begin();
      it_flags_node_sampler_affected = flags_node_sampler_affected_sequence.begin();
      while ( it_node_id != nodeIdSequence_.end() )
      {
        if ( !(*it_flags_node_sampler_affected) )
        {
          *it_flags_node_sampler_affected = (*rit_node_sampler_factory_order)->Create(pGraph_, it_node_id->front(), *it_node_sampler);
        }
        ++it_node_id;
        ++it_node_sampler;
        ++it_flags_node_sampler_affected;
      }
      ++rit_node_sampler_factory_order;
    }
  }


  Particle SMCSampler::fInitialize(Rng * pRng)
  {
    NodeValues init_particle_value(pGraph_->GetSize());
    for (NodeId id= 0; id<init_particle_value.size(); ++id)
    {
      init_particle_value[id] = pGraph_->GetValues()[id];
    }

    return Particle(init_particle_value, 0.0);
  }


  void SMCSampler::fMove(long lTime, Particle & lastParticle, Rng * pRng)
  {
    static Types<Types<NodeId>::Array>::Iterator & iterNodeId_ = iterNodeId();
    static Types<NodeSampler::Ptr>::Iterator & iterNodeSampler_ = iterNodeSampler();
    static Flags & sampledFlagsBefore_ = sampledFlagsBefore();
    static Flags & sampledFlagsAfter_ = sampledFlagsAfter();

    // sample current stochastic node
    NodeValues & moved_particle_value = *(lastParticle.GetValuePointer());
    sampledFlagsAfter_ = sampledFlagsBefore_;
    (*iterNodeSampler_)->SetAttributes(moved_particle_value, sampledFlagsAfter_, pRng);
    (*iterNodeSampler_)->Sample(iterNodeId_->front());

    // update particle log weight
    lastParticle.AddToLogWeight((*iterNodeSampler_)->LogWeight());

    // compute all logical children
    Types<NodeId>::Iterator it_logical_children = iterNodeId_->begin()+1;
    while(it_logical_children != iterNodeId_->end())
    {
      (*iterNodeSampler_)->Sample(*it_logical_children);
      ++it_logical_children;
    }
  }


  SMCSampler::SMCSampler(Size nbParticles, Graph * pGraph, Rng * pRng)
  : BaseType(nbParticles, SMC_HISTORY_NONE), Moves(&fInitialize, &fMove), pRng_(pRng), initialized_(false)
  {
    pGraph_ = pGraph;

    nodeSamplerFactoryInvOrder_.push_back(NodeSamplerFactory::Instance());
  }


  class BuildNodeIdSequenceVisitor : public ConstNodeVisitor
  {
  protected:
    typedef BuildNodeIdSequenceVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph * pGraph_;
    Types<Types<NodeId>::Array>::Array * pNodeIdSequence_;

  public:
    virtual void Visit(const ConstantNode & node) { };

    virtual void Visit(const StochasticNode & node)
    {
      if ( nodeIdDefined_ ) // TODO manage else case : throw exception
      {
        if ( ! pGraph_->GetObserved()[nodeId_] )
        {
          // push a new array whose first element is the stochastic node
          pNodeIdSequence_->push_back(Types<NodeId>::Array(1,nodeId_));
        }
      }
    };

    virtual void Visit(const LogicalNode & node)
    {
      if ( nodeIdDefined_ ) // TODO manage else case : throw exception
      {
        if ( ! pGraph_->GetObserved()[nodeId_] )
        {
          // push all the logical children back in the last array
          pNodeIdSequence_->back().push_back(nodeId_);
        }
      }
    };

    explicit BuildNodeIdSequenceVisitor(const Graph * pGraph, Types<Types<NodeId>::Array>::Array * pNodeIdSequence)
    : pGraph_(pGraph), pNodeIdSequence_(pNodeIdSequence) {};
  };


  void SMCSampler::buildNodeIdSequence()
  {
    // this will buil an array of NodeId arrays whose first
    // element are the unobserved stochastic nodes
    // followed by their logical children
    // in topological order
    BuildNodeIdSequenceVisitor build_node_id_sequence_vis(pGraph_, & nodeIdSequence_);
    pGraph_->VisitGraph(build_node_id_sequence_vis);
  }


  void SMCSampler::PrintSamplersSequence(std::ostream & os) const
  {
    for (Size  k = 0; k<nodeIdSequence_.size(); ++k)
    {
      os << k << ": node " << nodeIdSequence_[k].front() << ", " << nodeSamplerSequence_[k]->Name() << std::endl;
    }
  }


  void SMCSampler::Initialize()
  {
    static Types<Types<NodeId>::Array>::Iterator & iterNodeId_ = iterNodeId();
    static Types<NodeSampler::Ptr>::Iterator & iterNodeSampler_ = iterNodeSampler();
    static Flags & sampledFlagsBefore_ = sampledFlagsBefore();
    static Flags & sampledFlagsAfter_ = sampledFlagsAfter();

    buildNodeIdSequence();
    buildNodeSamplers();

    iterNodeId_ = nodeIdSequence_.begin();
    iterNodeSampler_ = nodeSamplerSequence_.begin();

    for (NodeId id= 0; id<sampledFlagsBefore_.size(); ++id)
    {
      sampledFlagsBefore_[id] = sampledFlagsAfter_[id] = pGraph_->GetObserved()[id];
    }

    T = 0;

    for(int i = 0; i < N; i++)
      pParticles[i] = Moves.DoInit(pRng_);

    //    if(htHistoryMode != ::SMC_HISTORY_NONE)
    //    {
    //      while(History.Pop());
    //    }

    initialized_ = true;
  }


  void SMCSampler::Iterate()
  {
    static Types<Types<NodeId>::Array>::Iterator & iterNodeId_ = iterNodeId();
    static Types<NodeSampler::Ptr>::Iterator & iterNodeSampler_ = iterNodeSampler();
    static Flags & sampledFlagsBefore_ = sampledFlagsBefore();
    static Flags & sampledFlagsAfter_ = sampledFlagsAfter();

    if ( iterNodeId_ != nodeIdSequence_.end() ) // TODO manage the else case
    {
      sampledFlagsBefore_ = sampledFlagsAfter_;

      if (nResampled)
        Resample(rtResampleMode);

      nAccepted = 0;

      //Move the particle set.
      for(int i = 0; i < N; i++)
        Moves.DoMove(T+1, pParticles[i], pRng_);

      //Normalize the weights to sensible values....
      double dMaxWeight = -std::numeric_limits<double>::infinity();
      for(int i = 0; i < N; i++)
        dMaxWeight = std::max(dMaxWeight, pParticles[i].GetLogWeight());
      for(int i = 0; i < N; i++)
        pParticles[i].SetLogWeight(pParticles[i].GetLogWeight() - (dMaxWeight));

      //Check if the ESS is below some reasonable threshold and resample if necessary.
      //A mechanism for setting this threshold is required.
      ess_ = GetESS();
      if (ess_ < dResampleThreshold)
        nResampled = 1;
      else
      {
        nResampled = 0;
        for (int i = 0; i<N ; ++i)
        {
          uRSIndices[i] = i;
        }
      }

      //      //the current particle set should be appended to the historical process.
      //      if(htHistoryMode != ::SMC_HISTORY_NONE)
      //      {
      //        History.PushBack(N, pParticles, nAccepted, HistoryFlags(nResampled), pGraph_->GetNodePtr(*iterNodeId_), Types<Size>::Array(uRSIndices, uRSIndices+N));
      //      }

      // Increment the evolution time.
      T++;

      ++iterNodeId_;
      ++iterNodeSampler_;
    }
  }


  void SMCSampler::Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n) const
  {
    featuresAcc.Init();
    for(Size i=0; i < Size(N); i++)
      featuresAcc.Push((*(pParticles[i].GetValue()[nodeId]))[n], exp(pParticles[i].GetLogWeight()));
  }


  void SMCSampler::Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n) const
  {
    featuresAcc.Init();
    for(Size i=0; i < Size(N); i++)
      featuresAcc.Push((*(pParticles[i].GetValue()[nodeId]))[n], exp(pParticles[i].GetLogWeight()));
  }

  void SMCSampler::Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc) const
  {
    featuresAcc.Init(pGraph_->GetNode(nodeId).DimPtr());
    for(Size i=0; i < Size(N); i++)
      featuresAcc.Push(pParticles[i].GetValue()[nodeId], exp(pParticles[i].GetLogWeight()));
  }


  void SMCSampler::PrintSamplerState(std::ostream & os) const
  {
    const Types<Types<NodeId>::Array>::ConstIterator & iterNodeId_ = iterNodeId();

    if (!initialized_)
      os << "Non initialized" << std::endl;
    else if ( iterNodeId_ == nodeIdSequence_.begin() )
      os << "Initialized" << std::endl;
    else
    {
      Size old_prec = os.precision();
      os.precision(4);
      Size k = std::distance(nodeIdSequence_.begin(), iterNodeId_-1);
      os << k << ": ESS/N = " << std::fixed << ess_/N;
      if (iterNodeId_ != nodeIdSequence_.end() && nResampled)
        os << ", Resampled";
      os << std::endl;
      os.precision(old_prec);
    }
  };

  //  template<typename Features>
  //  void SMCSampler::Accumulate(NodeId nodeId, VectorAccumulator<Features> & featuresAcc) const
  //  {
  //    featuresAcc.Clear();
  //    for(Size i=0; i < Size(N); i++)
  //      featuresAcc.Push(pParticles[i].GetValue()[nodeId], exp(pParticles[i].GetLogWeight()));
  ////    featuresAcc.SetDim(pGraph_->GetNode(nodeId).Dim());
  //  }


} /* namespace Biips */
