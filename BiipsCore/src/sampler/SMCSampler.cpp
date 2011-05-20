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

#include <boost/random/uniform_real.hpp>
#include <boost/random/discrete_distribution_sw_2009.hpp>

namespace Biips
{

  std::list<std::pair<NodeSamplerFactory::Ptr, Bool> > & SMCSampler::NodeSamplerFactories()
  {
    static std::list<std::pair<NodeSamplerFactory::Ptr, Bool> > ans;
    return ans;
  }


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
          it_sampler_factory->first->Create(pGraph_, it_node_id->front(), *it_node_sampler);
      }
    }

    // affect default prior NodeSampler to all non affected nodes
    it_node_id = nodeIdSequence_.begin();
    it_node_sampler = nodeSamplerSequence_.begin();
    for(; it_node_id != nodeIdSequence_.end();
        ++it_node_id, ++it_node_sampler)
    {
      if ( !(*it_node_sampler) )
        NodeSamplerFactory::Instance()->Create(pGraph_, it_node_id->front(), *it_node_sampler);
    }
  }


  SMCSampler::SMCSampler(Size nbParticles, Graph * pGraph, Rng * pRng)
  : nParticles_(nbParticles), pGraph_(pGraph), pRng_(pRng),
    resampleMode_(SMC_RESAMPLE_STRATIFIED), resampleThreshold_(0.5 * nbParticles),
    rsWeights_(nbParticles), rsCount_(nbParticles), rsIndices_(nbParticles),
    sampledFlagsBefore_(pGraph->GetSize()), built_(false), initialized_(false)
  {
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
      os << k << ": node " << nodeIdSequence_[k].front() << ", " << nodeSamplerSequence_[k]->Name() << std::endl;
  }


  Types<std::pair<NodeId, String> >::Array SMCSampler::GetSamplersSequence() const
  {
    Types<std::pair<NodeId, String> >::Array ans;
    for (Size k = 0; k<nodeIdSequence_.size(); ++k)
      ans.push_back(std::make_pair(nodeIdSequence_[k].front(), nodeSamplerSequence_[k]->Name()));

    return ans;
  }


  void SMCSampler::SetResampleParams(ResampleType rsMode, Scalar threshold)
  {
    resampleMode_ = rsMode;
    resampleThreshold_ = threshold < 1.0 ? threshold * nParticles_ : nParticles_;
  }


  void SMCSampler::moveParticle(Particle & lastParticle, Rng * pRng)
  {
    // sample current stochastic node
    NodeValues & moved_particle_value = lastParticle.Value();
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

  Scalar SMCSampler::normalizeWeights()
  {
    //Normalize the weights to sensible values....
    Scalar max_weight = particles_[0].GetLogWeight();
    for(Size i = 1; i < nParticles_; i++)
      max_weight = std::max(max_weight, particles_[i].GetLogWeight());
    for(Size i = 0; i < nParticles_; i++)
      particles_[i].SetLogWeight(particles_[i].GetLogWeight() - max_weight);

    return max_weight;
  }


  Scalar SMCSampler::sumOfWeightsAndEss()
  {
    long double sum = 0.0;
    for(Size i = 0; i < nParticles_; i++)
      sum += expl(particles_[i].GetLogWeight());

    long double sumsq = 0;
    for(Size i = 0; i < nParticles_; i++)
      sumsq += expl(2.0*(particles_[i].GetLogWeight()));

    ess_ = expl(-logl(sumsq) + 2*logl(sum));

    return sum;
  }


  void SMCSampler::Initialize()
  {
    if (!built_)
      throw LogicError("Can not initialize SMCSampler: not built.");

    t_ = 0;

    iterNodeId_ = nodeIdSequence_.begin();
    iterNodeSampler_ = nodeSamplerSequence_.begin();

    for(NodeId id = 0; id < pGraph_->GetSize(); ++id)
      sampledFlagsBefore_[id] = pGraph_->GetObserved()[id];

    //Initialize the particle set.
    NodeValues init_node_values(pGraph_->GetSize());
    for(NodeId id = 0; id < pGraph_->GetSize(); ++id)
      init_node_values[id] = pGraph_->GetValues()[id];
    particles_.assign(nParticles_, Particle(init_node_values, 0.0));

    //Move the particle set.
    for(Size i = 0; i < nParticles_; i++)
      moveParticle(particles_[i], pRng_);

    //Normalize the weights to sensible values....
    Scalar max_weight = normalizeWeights();

    sumOfWeights_ = sumOfWeightsAndEss();

    // increment the normalizing constant
    logNormConst_ = log(sumOfWeights_) - log(nParticles_) + max_weight;

    //Check if the ESS is below some reasonable threshold.
    //A mechanism for setting this threshold is required.
    resampled_ = ess_ < resampleThreshold_;

    initialized_ = true;
  }


  void SMCSampler::Iterate()
  {
    if (!initialized_)
      throw LogicError("Can not iterate SMCSampler: not initialized.");

    if ( iterNodeId_+1 == nodeIdSequence_.end() )
      throw LogicError("Can not iterate SMCSampler: the sequence have reached the end.");

    // Increment the evolution time.
    ++t_;

    ++iterNodeId_;
    ++iterNodeSampler_;

    sampledFlagsBefore_ = sampledFlagsAfter_;

    //Resample if necessary.
    if (resampled_)
    {
      resample(resampleMode_);
    }

    //Move the particle set.
    for(Size i = 0; i < nParticles_; i++)
      moveParticle(particles_[i], pRng_);

    //Normalize the weights to sensible values....
    Scalar max_weight = normalizeWeights();

    //Check if the ESS is below some reasonable threshold.
    //A mechanism for setting this threshold is required.
    Scalar sum = sumOfWeightsAndEss();

    //Check if the ESS is below some reasonable threshold.
    //A mechanism for setting this threshold is required.
    resampled_ = ess_ < resampleThreshold_;

    // increment the normalizing constant
    logNormConst_ += log(sum) - log(sumOfWeights_) + max_weight;

    sumOfWeights_ = sum;
  }


  // COPY: copied and pasted from SMCTC sampler<Space>::resample method
  // and then modified to fit Biips code
  // COPY: ********** from here **********
  void SMCSampler::resample(ResampleType rsMode)
  {
    //Resampling is done in place.

    for(Size i = 0; i < nParticles_; ++i)
      rsCount_[i] = 0;

    typedef boost::random::discrete_distribution<Int ,Scalar> CategoricalDist;
    typedef boost::uniform_real<Scalar> UniformDist;

    //First obtain a count of the number of children each particle has.
    switch(rsMode)
    {
      case SMC_RESAMPLE_MULTINOMIAL:
      {
        //Sample from a suitable multinomial vector
        for(Size i = 0; i < nParticles_; ++i)
          rsWeights_[i] = particles_[i].GetWeight();

        CategoricalDist dist(rsWeights_.begin(), rsWeights_.end());
        for (Size i=0; i<nParticles_; ++i)
          rsCount_[dist(pRng_->GetGen())]++;
        break;
      }

      case SMC_RESAMPLE_RESIDUAL:
      {
        //Sample from a suitable multinomial vector and add the integer replicate
        //counts afterwards.
        for(Size i = 0; i < nParticles_; ++i)
          rsWeights_[i] = particles_[i].GetWeight();

        Size multinomial_count = nParticles_;
        for(Size i = 0; i < nParticles_; ++i)
        {
          rsWeights_[i] = nParticles_*rsWeights_[i] / sumOfWeights_;
          rsIndices_[i] = floor(rsWeights_[i]); //Reuse temporary storage.
          rsWeights_[i] = (rsWeights_[i] - rsIndices_[i]);
          multinomial_count -= rsIndices_[i];
        }

        CategoricalDist dist(rsWeights_.begin(), rsWeights_.end());
        for (Size i=0; i<multinomial_count; ++i)
          rsCount_[dist(pRng_->GetGen())]++;

        for(Size i = 0; i < nParticles_; ++i)
          rsCount_[i] += rsIndices_[i];
        break;
      }

      case SMC_RESAMPLE_STRATIFIED:
      default:
      {
        // Procedure for stratified sampling
        Scalar weight_cumulative = 0.0;
        //Generate a random number between 0 and 1/nParticles_ times the sum of the weights
        UniformDist dist(0.0, 1.0 / nParticles_);
        Scalar rand_unif = dist(pRng_->GetGen());

        Size j = 0, k = 0;
        for(Size i = 0; i < nParticles_; ++i)
          rsCount_[i] = 0;

        weight_cumulative = particles_[0].GetWeight() / sumOfWeights_;
        while(j < nParticles_)
        {
          while((weight_cumulative - rand_unif) > Scalar(j)/nParticles_ && j < nParticles_)
          {
            rsCount_[k]++;
            j++;
            rand_unif = dist(pRng_->GetGen());
          }
          k++;
          weight_cumulative += particles_[k].GetWeight() / sumOfWeights_;
        }
        break;
      }

      case SMC_RESAMPLE_SYSTEMATIC:
      {
        // Procedure for stratified sampling but with a common RV for each stratum
        Scalar weight_cumulative = 0.0;
        //Generate a random number between 0 and 1/nParticles_ times the sum of the weights
        UniformDist dist(0.0, 1.0 / nParticles_);
        Scalar rand_unif = dist(pRng_->GetGen());

        Size j = 0, k = 0;
        for(Size i = 0; i < nParticles_; ++i)
          rsCount_[i] = 0;

        weight_cumulative = particles_[0].GetWeight() / sumOfWeights_;
        while(j < nParticles_)
        {
          while((weight_cumulative - rand_unif) > Scalar(j)/nParticles_ && j < nParticles_)
          {
            rsCount_[k]++;
            j++;
          }
          k++;
          weight_cumulative += particles_[k].GetWeight() / sumOfWeights_;
        }
        break;

      }
    }

    //Map count to indices to allow in-place resampling
    for (Size i=0, j=0; i<nParticles_; ++i)
    {
      if (rsCount_[i]>0)
      {
        rsIndices_[i] = i;
        while (rsCount_[i]>1)
        {
          while (rsCount_[j]>0) ++j; // find next free spot
          rsIndices_[j++] = i; // assign index
          --rsCount_[i]; // decrement number of remaining offsprings
        }
      }
    }

    //Perform the replication of the chosen.
    for(Size i = 0; i < nParticles_ ; ++i)
    {
      if(rsIndices_[i] != i)
        particles_[i].Value() = particles_[rsIndices_[i]].GetValue();
      particles_[i].SetLogWeight(0.0);
    }
    // COPY: ********** to here **********
    sumOfWeights_ = nParticles_;
  }


  void SMCSampler::Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n) const
  {
    featuresAcc.Init();
    for(Size i=0; i < nParticles_; i++)
      featuresAcc.Push((*(particles_[i].GetValue()[nodeId]))[n], particles_[i].GetWeight());
  }


  void SMCSampler::Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n) const
  {
    featuresAcc.Init();
    for(Size i=0; i < nParticles_; i++)
      featuresAcc.Push((*(particles_[i].GetValue()[nodeId]))[n], particles_[i].GetWeight());
  }

  void SMCSampler::Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc) const
  {
    featuresAcc.Init(pGraph_->GetNode(nodeId).DimPtr());
    for(Size i=0; i < nParticles_; i++)
      featuresAcc.Push(particles_[i].GetValue()[nodeId], particles_[i].GetWeight());
  }


  void SMCSampler::SetMonitorWeights(FilterMonitor & monitor) const
  {
    monitor.SetWeights(particles_, ess_, sumOfWeights_, logNormConst_);
  }


  void SMCSampler::SetMonitorNodeValues(NodeId nodeId, FilterMonitor & monitor) const
  {
    monitor.SetNodeValues(nodeId, particles_);
  }

  void SMCSampler::PrintSamplerState(std::ostream & os) const
  {
    if (!initialized_)
      os << "Not initialized" << std::endl;
    else
    {
      Size old_prec = os.precision();
      os.precision(4);
      os << t_ << ": ESS/N = " << std::fixed << ess_/nParticles_;
      if (!AtEnd() && resampled_)
        os << ", Resampled";
      os << std::endl;
      os.precision(old_prec);
    }
  };

  //  template<typename Features>
  //  void SMCSampler::Accumulate(NodeId nodeId, VectorAccumulator<Features> & featuresAcc) const
  //  {
  //    featuresAcc.Clear();
  //    for(Size i=0; i < nParticles_; i++)
  //      featuresAcc.Push(particles_[i].GetValue()[nodeId], exp(particles_[i].GetLogWeight()));
  // //    featuresAcc.SetDim(pGraph_->GetNode(nodeId).Dim());
  //  }


} /* namespace Biips */
