//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
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

/*! \file DiscreteOptimal.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "samplers/DiscreteOptimal.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "sampler/LogLikeVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "distributions/DCat.hpp"

namespace Biips
{

  const String DiscreteOptimal::NAME_ = "Discrete Optimal";

  void DiscreteOptimal::sample(const StochasticNode & node)
  {
    NodeValues node_values(graph_.GetSize());
    Flags sampled_flags(graph_.GetSize());

    NodeSampler node_sampler(graph_);

    NumArray::Array prior_param_values = getParamValues(nodeId_, graph_, *this);

    ValArray post_param(prior_param_values[0].Dim().Length());

    ValArray::Ptr k_chosen(new ValArray(1));

    for (Size k = 1; k <= post_param.size(); ++k)
    {
      for (NodeId id = 0; id < graph_.GetSize(); ++id)
      {
        node_values[id] = nodeValuesMap()[id];
        sampled_flags[id] = sampledFlagsMap()[id];
      }
      k_chosen->ScalarView() = Scalar(k);
      node_values[nodeId_] = k_chosen;
      sampled_flags[nodeId_] = true;

      node_sampler.SetMembers(node_values, sampled_flags, pRng_);

      Scalar log_like = getLogLikelihood(graph_, nodeId_, node_sampler);

      Scalar post_param_k = std::exp(std::log(prior_param_values[0].Values()[k
          - 1]) + log_like);
      if (isNan(post_param_k))
        throw RuntimeError("Failure to calculate log posterior parameter.");

      post_param[k - 1] = post_param_k;
    }

    NumArray::Array post_param_values(1);
    post_param_values[0].SetPtr(prior_param_values[0].DimPtr(), &post_param);

    //allocate memory
    nodeValuesMap()[nodeId_].reset(new ValArray(1));
    //sample
    DCat::Instance()->Sample(*nodeValuesMap()[nodeId_],
                             post_param_values,
                             NULL_NUMARRAYPAIR,
                             *pRng_); // FIXME Boundaries

    sampledFlagsMap()[nodeId_] = true;

    logIncrementalWeight_ = std::log(post_param_values[0].Values().Sum());
    if (isNan(logIncrementalWeight_))
      throw RuntimeError("Failure to calculate log incremental weight.");
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

      if (node.PriorName() != DCat::Instance()->Name())
        return;

      // FIXME
      if (node.IsBounded())
        return;

      GraphTypes::LikelihoodChildIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end)
          = graph_.GetLikelihoodChildren(nodeId_);

      for (; it_offspring != it_offspring_end; ++it_offspring)
      {
        canSample_ = true;
        break;
      }
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

  Bool DiscreteOptimalFactory::Create(const Graph & graph,
                                      NodeId nodeId,
                                      BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleDiscreteOptimalVisitor can_sample_vis(graph);

    graph.VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if (flag_created)
    {
      pNodeSamplerInstance
          = NodeSamplerFactory::CreatedPtr(new CreatedType(graph));
    }

    return flag_created;
  }

  DiscreteOptimalFactory::Ptr
      DiscreteOptimalFactory::pDiscreteOptimalFactoryInstance_(new DiscreteOptimalFactory());
}
