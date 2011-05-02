//                                               -*- C++ -*-
/*! \file SMCSampler.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: part of this file is copied and pasted from SMCTC sampler<Space> class
 */

#include "sampler/SMCSampler.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodeSampler.hpp"
#include "graph/ConstantNode.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "sampler/Accumulator.hpp"
#include "model/Monitor.hpp"


namespace Biips
{

  Graph * SMCSampler::pGraph_;

  std::list<std::pair<NodeSamplerFactory::Ptr, Bool> > & SMCSampler::NodeSamplerFactories()
  {
    static std::list<std::pair<NodeSamplerFactory::Ptr, Bool> > ans;
    return ans;
  }

  Flags & SMCSampler::sampledFlagsBefore() { static Flags ans(pGraph_->GetSize()); return ans; };
  Flags & SMCSampler::sampledFlagsAfter() { static Flags ans(pGraph_->GetSize()); return ans; };

  void SMCSampler::buildNodeSamplers()
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
        {
          it_sampler_factory->first->Create(pGraph_, it_node_id->front(), *it_node_sampler);
        }
      }
    }

    // affect default prior NodeSampler to all non affected nodes
    it_node_id = nodeIdSequence_.begin();
    it_node_sampler = nodeSamplerSequence_.begin();
    for(; it_node_id != nodeIdSequence_.end();
        ++it_node_id, ++it_node_sampler)
    {
      if ( !(*it_node_sampler) )
      {
        NodeSamplerFactory::Instance()->Create(pGraph_, it_node_id->front(), *it_node_sampler);
      }
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
    // sample current stochastic node
    NodeValues & moved_particle_value = *(lastParticle.GetValuePointer());
    sampledFlagsAfter() = sampledFlagsBefore();
    (*iterNodeSampler())->SetAttributes(moved_particle_value, sampledFlagsAfter(), pRng);
    (*iterNodeSampler())->Sample(iterNodeId()->front());

    // update particle log weight
    lastParticle.AddToLogWeight((*iterNodeSampler())->LogWeight());

    // compute all logical children
    Types<NodeId>::Iterator it_logical_children = iterNodeId()->begin()+1;
    while(it_logical_children != iterNodeId()->end())
    {
      (*iterNodeSampler())->Sample(*it_logical_children);
      ++it_logical_children;
    }
  }


  SMCSampler::SMCSampler(Size nbParticles, Graph * pGraph, Rng * pRng)
  : BaseType(nbParticles, SMC_HISTORY_NONE), Moves(&fInitialize, &fMove), pRng_(pRng), initialized_(false)
  {
    pGraph_ = pGraph;
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
        if ( !pGraph_->GetObserved()[nodeId_] )
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
        if ( !pNodeIdSequence_->empty() && !pGraph_->GetObserved()[nodeId_] )
        {
          // push all the logical children back in the last array
          pNodeIdSequence_->back().push_back(nodeId_);
        }
      }
    };

    BuildNodeIdSequenceVisitor(const Graph * pGraph, Types<Types<NodeId>::Array>::Array * pNodeIdSequence)
    : pGraph_(pGraph), pNodeIdSequence_(pNodeIdSequence) {};
  };


  void SMCSampler::buildNodeIdSequence()
  {
    // this will build an array of NodeId arrays whose first
    // element are the unobserved stochastic nodes
    // followed by their logical children
    // in topological order
    BuildNodeIdSequenceVisitor build_node_id_sequence_vis(pGraph_, &nodeIdSequence_);
    pGraph_->VisitGraph(build_node_id_sequence_vis);
  }


  void SMCSampler::PrintSamplersSequence(std::ostream & os) const
  {
    for (Size  k = 0; k<nodeIdSequence_.size(); ++k)
    {
      os << k << ": node " << nodeIdSequence_[k].front() << ", " << nodeSamplerSequence_[k]->Name() << std::endl;
    }
  }


  Types<std::pair<NodeId, String> >::Array SMCSampler::GetSamplersSequence() const
  {
    Types<std::pair<NodeId, String> >::Array ans;
    for (Size k = 0; k<nodeIdSequence_.size(); ++k)
    {
      ans.push_back(std::make_pair(nodeIdSequence_[k].front(), nodeSamplerSequence_[k]->Name()));
    }
    return ans;
  }


  void SMCSampler::Initialize()
  {
    buildNodeIdSequence();
    buildNodeSamplers();

    iterNodeId() = nodeIdSequence_.begin();
    iterNodeSampler() = nodeSamplerSequence_.begin();

    for (NodeId id= 0; id<sampledFlagsBefore().size(); ++id)
    {
      sampledFlagsBefore()[id] = sampledFlagsAfter()[id] = pGraph_->GetObserved()[id];
    }

    T = 0;

    for(int i = 0; i < N; i++)
    {
      pParticles[i] = Moves.DoInit(pRng_);
      pParticles[i].SetLogWeight(-log(Scalar(N)));
    }

    logNormConst_ = 0.0;

    initialized_ = true;
  }


  void SMCSampler::Iterate()
  {
    if (!initialized_)
      throw LogicError("Can not iterate SMCSampler: not initialized.");

    if ( iterNodeId() == nodeIdSequence_.end() )
      throw LogicError("Can not iterate SMCSampler: the sequence have reached the end.");

    sampledFlagsBefore() = sampledFlagsAfter();

    if (nResampled)
      Resample(rtResampleMode);

    // COPY: copied and pasted from SMCTC sampler<Space>::IterateESS and GetESS methods
    // and then modified to fit Biips code
    // COPY: ********** from here **********

    nAccepted = 0;

    //Move the particle set.
    for(int i = 0; i < N; i++)
      Moves.DoMove(T+1, pParticles[i], pRng_);

//    //Normalize the weights to sensible values....
//    double dMaxWeight = -std::numeric_limits<double>::infinity();
//    for(int i = 0; i < N; i++)
//      dMaxWeight = std::max(dMaxWeight, pParticles[i].GetLogWeight());
//    for(int i = 0; i < N; i++)
//      pParticles[i].SetLogWeight(pParticles[i].GetLogWeight() - (dMaxWeight));

    //Normalize the weights to sum to 1
    long double sum = 0.0;
    for(int i = 0; i < N; i++)
      sum += expl(pParticles[i].GetLogWeight());

    for(int i = 0; i < N; i++)
      pParticles[i].SetLogWeight(pParticles[i].GetLogWeight() - log(sum));

    //Check if the ESS is below some reasonable threshold and resample if necessary.
    //A mechanism for setting this threshold is required.

    long double sumsq = 0;
    for(int i = 0; i < N; i++)
      sumsq += expl(2.0*(pParticles[i].GetLogWeight()));

    ess_ = 1/sumsq;

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

    // Increment the evolution time.
    T++;

    // COPY: ********** to here **********

    // increment the normalizing constant
    logNormConst_ += log(sum);


    ++iterNodeId();
    ++iterNodeSampler();
  }


  void SMCSampler::Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n) const
  {
    featuresAcc.Init();

    //Normalize the weights to sensible values....
    double dMaxWeight = -std::numeric_limits<double>::infinity();
    for(int i = 0; i < N; i++)
      dMaxWeight = std::max(dMaxWeight, pParticles[i].GetLogWeight());

    for(Size i=0; i < Size(N); i++)
      featuresAcc.Push((*(pParticles[i].GetValue()[nodeId]))[n], exp(pParticles[i].GetLogWeight() - dMaxWeight));
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


  void SMCSampler::MonitorNode(NodeId nodeId, Monitor & monitor)
  {
    for (Size i=0; i < Size(N); i++)
      monitor.PushParticle(nodeId, pParticles[i].GetValue()[nodeId], exp(pParticles[i].GetLogWeight()));
  }

  void SMCSampler::PrintSamplerState(std::ostream & os) const
  {
    const Types<Types<NodeId>::Array>::ConstIterator & iter_node_id = iterNodeId();

    if (!initialized_)
      os << "Non initialized" << std::endl;
    else if ( iter_node_id == nodeIdSequence_.begin() )
      os << "Initialized" << std::endl;
    else
    {
      Size old_prec = os.precision();
      os.precision(4);
      Size k = std::distance(nodeIdSequence_.begin(), iter_node_id-1);
      os << k << ": ESS/N = " << std::fixed << ess_/N;
      if (iter_node_id != nodeIdSequence_.end() && nResampled)
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
  // //    featuresAcc.SetDim(pGraph_->GetNode(nodeId).Dim());
  //  }


} /* namespace Biips */
