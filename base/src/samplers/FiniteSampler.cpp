//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file FiniteSampler.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "samplers/FiniteSampler.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "sampler/LogLikeVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "sampler/DataNodeSampler.hpp"
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  const String FiniteSampler::NAME_ = "Finite";

  void FiniteSampler::sample(const StochasticNode & node)
  {
    // Size of the support
    Size size = upper_ - lower_ + 1;

    // temporary node_values and sampled_flags that will be used for the calculations
    NodeValues node_values;
    Flags sampled_flags;

    NodeSampler node_sampler(graph_);
    node_sampler.SetMembers(node_values, sampled_flags, pRng_);

    // prior parameters
    NumArray::Array prior_param_values = getParamValues(nodeId_, graph_, *this);
    NumArray::Pair prior_bound_values = getBoundValues(nodeId_, graph_, *this);

    // vector of the posterior probabilities for each value of the support
    ValArray probas(size);

    ValArray::Ptr k_val(new ValArray(1));
    NumArray k_num(node.DimPtr().get(), k_val.get());

    Scalar max_logprobas = BIIPS_NEGINF;
    for (Size k = 1; k <= size; ++k)
    {
      // reset node_values and sampled_flags
      node_values.assign(nodeValuesMap().begin(), nodeValuesMap().end());
      sampled_flags.assign(sampledFlagsMap().begin(), sampledFlagsMap().end());

      // assign k value to current node
      k_val->ScalarView() = Scalar(k);
      node_values[nodeId_] = k_val;
      sampled_flags[nodeId_] = true;

      // get log_prior
      Scalar log_prior = node.PriorPtr()->LogDensity(k_num,
                                                     prior_param_values,
                                                     prior_bound_values);

      // get log_like
      Scalar log_like = getLogLikelihood(graph_, nodeId_, node_sampler);


      probas[k - 1] = log_prior + log_like;
      if (isNan(probas[k - 1]))
        throw RuntimeError("Failure to calculate log posterior parameter.");

      max_logprobas = std::max(max_logprobas, probas[k - 1]);
    }

    //Transform log-proba to probas, avoiding overflow
    Scalar sum_probas = 0;
    for (Size i = 0; i < size; ++i)
    {
      probas[i] = std::exp(probas[i] - max_logprobas);
      sum_probas += probas[i];
    }

    if (!isFinite(sum_probas))
      throw NodeError(nodeId_, "Cannot normalize density");

    // sample
    typedef boost::random::discrete_distribution<Int, Scalar> DistType;
    DistType dist(probas.begin(), probas.end());
    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(pRng_->GetGen(), dist);

    Scalar ivalue = Scalar(lower_ + gen());

    //allocate memory
    nodeValuesMap()[nodeId_].reset(new ValArray(1, ivalue));
    sampledFlagsMap()[nodeId_] = true;

    // compute log incremental weight
    logIncrementalWeight_ = std::log(sum_probas) + max_logprobas;
    if (isNan(logIncrementalWeight_))
      throw NodeError(nodeId_, "Failure to calculate log incremental weight.");
  }


  class CanSampleDiscreteOptimalVisitor: public ConstNodeVisitor
  {
  protected:
    typedef GraphTypes::StochasticParentIterator StochasticParentNodeIdIterator;

    const Graph & graph_;
    Bool canSample_;

    virtual void visit(const StochasticNode & node)
    {
      canSample_ = false;

      if (graph_.GetObserved()[nodeId_])
        throw LogicError("CanSampleDiscreteOptimalVisitor can not visit observed node: node id sequence of the forward sampler may be bad.");

      //Node must be scalar with discrete-valued distribution of full rank
      if (!graph_.GetDiscrete()[nodeId_])
        return;
      if (!node.Dim().IsScalar())
        return;

      Types<NodeId>::Array par = node.Parents();
      if (node.IsUpperBounded())
        par.pop_back();
      if (node.IsLowerBounded())
        par.pop_back();

      // TODO: use getParamDims
      Types<DimArray::Ptr>::Array param_dims(par.size());
      for (Size i = 0; i < par.size(); ++i)
        param_dims[i] = graph_.GetNode(par[i]).DimPtr();

      if (node.PriorPtr()->DegreeOfFreedom(param_dims) == 0)
        return;

      //Support must be fixed
      if (!isSupportFixed(nodeId_, graph_))
        return;

      //Distribution cannot be unbounded
      ValArray lower(1), upper(1);

      getFixedSupportValues(lower, upper, nodeId_, graph_);
      if (!isFinite(lower[0]) || !isFinite(upper[0]))
        return;

      //We don't want too many possibilities
      Scalar n = upper[0] - lower[0] + 1;
      if (n <= 1.0 || n > 20.0) //FIXME: totally arbitrary
        return;

      //Check there is at least one observed stochastic children
      GraphTypes::LikelihoodChildIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end) =
          graph_.GetLikelihoodChildren(nodeId_);

      canSample_ = (std::distance(it_offspring, it_offspring_end) > 0);
    }

  public:
    Bool CanSample() const
    {
      return canSample_;
    }

    CanSampleDiscreteOptimalVisitor(const Graph & graph) :
        graph_(graph), canSample_(false)
    {
    }
  };

  Bool FiniteSamplerFactory::Create(const Graph & graph,
                                    NodeId nodeId,
                                    BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleDiscreteOptimalVisitor can_sample_vis(graph);

    graph.VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if (flag_created)
    {
      ValArray lower(1), upper(1);

      getFixedSupportValues(lower, upper, nodeId, graph);

      pNodeSamplerInstance =
          NodeSamplerFactory::CreatedPtr(new CreatedType(graph,
                                                         static_cast<int>(lower[0]),
                                                         static_cast<int>(upper[0])));
    }

    return flag_created;
  }

  FiniteSamplerFactory::Ptr FiniteSamplerFactory::pFiniteSamplerFactoryInstance_(new FiniteSamplerFactory());
}
