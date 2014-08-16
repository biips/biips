//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file LogLikeVisitor.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/LogLikeVisitor.hpp"
#include "graph/NodeVisitor.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodeSampler.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "sampler/NodesRelationVisitor.hpp"

namespace Biips
{

  class LogLikeVisitor: public ConstNodeVisitor
  {
  protected:
    typedef LogLikeVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph & graph_;
    NodeId myId_;
    NodeSampler & nodeSampler_;
    Scalar logLikelihood_;

    virtual void visit(const StochasticNode & node);

  public:
    Scalar Value() const
    {
      return logLikelihood_;
    }

    LogLikeVisitor(const Graph & graph, NodeId myId, NodeSampler & nodeSampler);
  };

  void LogLikeVisitor::visit(const StochasticNode & node)
  {
    NumArray x_value(node.DimPtr().get(), graph_.GetValues()[nodeId_].get());
    NumArray::Array param_values = getParamValues(nodeId_,
                                                    graph_,
                                                    nodeSampler_);
    NumArray::Pair bound_values = getBoundValues(nodeId_,
                                                   graph_,
                                                   nodeSampler_);
    Scalar log_like = node.LogPriorDensity(x_value, param_values, bound_values);
    if (isNan(log_like))
      throw NodeError(nodeId_, "Failure to calculate log density.");

    logLikelihood_ += log_like;
    if (isNan(logLikelihood_))
      throw RuntimeError("Failure to calculate log likelihood.");
  }

  LogLikeVisitor::LogLikeVisitor(const Graph & graph,
                                 NodeId myId,
                                 NodeSampler & nodeSampler) :
    graph_(graph), myId_(myId), nodeSampler_(nodeSampler), logLikelihood_(0.0)
  {
  }

  Scalar getLogLikelihood(const Graph & graph,
                          NodeId myId,
                          NodeSampler & nodeSampler)
  {
    LogLikeVisitor log_like_vis(graph, myId, nodeSampler);
    GraphTypes::LikelihoodChildIterator it_offspring, it_offspring_end;
    boost::tie(it_offspring, it_offspring_end)
        = graph.GetLikelihoodChildren(myId);
    while (it_offspring != it_offspring_end)
    {
      graph.VisitNode(*it_offspring, log_like_vis);
      ++it_offspring;
    }
    return log_like_vis.Value();
  }

}
