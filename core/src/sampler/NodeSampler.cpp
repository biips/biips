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

/*! \file NodeSampler.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/NodeSampler.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/LogLikeVisitor.hpp"
#include "graph/ConstantNode.hpp"
#include "graph/LogicalNode.hpp"
#include "graph/StochasticNode.hpp"

namespace Biips
{

  const String NodeSampler::NAME_ = "Prior";

  void NodeSampler::visit(const LogicalNode & node)
  {
    if (!membersSet_)
      throw LogicError("NodeSampler can not visit LogicalNode: members not set.");

    // TODO double check the need of this
//    if (sampledFlagsMap()[nodeId_])
//      return;

    NumArray::Array params = getParamValues(nodeId_, graph_, *this);

    // allocate memory
    if (!nodeValuesMap()[nodeId_])
      nodeValuesMap()[nodeId_].reset(new ValArray(node.Dim().Length()));

    // FIXME
    // evaluate
    try
    {
      node.Eval(*nodeValuesMap()[nodeId_], params);
    }
    catch (RuntimeError & err)
    {
      throw NodeError(nodeId_, String(err.what()));
    }

    sampledFlagsMap()[nodeId_] = true;
  }

  void NodeSampler::visit(const StochasticNode & node)
  {
    if (!membersSet_)
      throw LogicError("NodeSampler can not visit StochasticNode: members not set.");

    // FIXME
//    try
//    {
    sample(node);
//    }
//    catch (RuntimeError & err)
//    {
//      throw NodeError(nodeId_, String("Failure to sample stochastic node.\n") + err.what());
//    }
  }

  void NodeSampler::sample(const StochasticNode & node)
  {
    NumArray::Array param_values = getParamValues(nodeId_, graph_, *this);
    NumArray::Pair bound_values = getBoundValues(nodeId_, graph_, *this);

    // allocate memory
    if (!nodeValuesMap()[nodeId_])
      nodeValuesMap()[nodeId_].reset(new ValArray(node.Dim().Length()));

    if (!pRng_)
      throw LogicError("NodeSampler can not sample StochasticNode: Rng pointer is null.");

    // sample
    try
    {
      node.Sample(*nodeValuesMap()[nodeId_],
                  param_values,
                  bound_values,
                  *pRng_);
    }
    catch (RuntimeError & err)
    {
      throw NodeError(nodeId_, String(err.what()));
    }

    sampledFlagsMap()[nodeId_] = true;
    logIncrementalWeight_ = getLogLikelihood(graph_, nodeId_, *this);
  }

  void NodeSampler::Sample(NodeId nodeId)
  {
    graph_.VisitNode(nodeId, *this);
  }

  NodeSamplerFactory::Ptr NodeSamplerFactory::pFactoryInstance_(new NodeSamplerFactory());

}
