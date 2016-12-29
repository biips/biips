//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
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

/*! \file   DataNodeSampler.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/DataNodeSampler.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "graph/StochasticNode.hpp"

namespace Biips
{

  const String DataNodeSampler::NAME_ = "Data sampler";

  void DataNodeSampler::sample(const StochasticNode & node)
  {
    NumArray::Array param_values = getParamValues(nodeId_, graph_, *this);
    NumArray::Pair bound_values = getBoundValues(nodeId_, graph_, *this);

    // allocate memory
    if (!nodeValuesMap()[nodeId_])
      nodeValuesMap()[nodeId_].reset(new ValArray(node.Dim().Length()));

    if (!pRng_)
      throw LogicError(
          "DataNodeSampler can not sample StochasticNode: Rng pointer is null.");

    // sample
    try
    {
      node.Sample(*nodeValuesMap()[nodeId_], param_values, bound_values,
                  *pRng_);
    }
    catch (RuntimeError & err)
    {
      throw NodeError(nodeId_, String(err.what()));
    }

    sampledFlagsMap()[nodeId_] = true;
  }
} /* namespace Biips */
