//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
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

/*! \file StochasticNode.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "graph/StochasticNode.hpp"
#include "graph/NodeVisitor.hpp"

namespace Biips
{

  static Types<NodeId>::Array mkParents(const Types<NodeId>::Array & parameters,
                                        NodeId lower,
                                        NodeId upper)
  {
    //Add bounds to vector of parents, if they are non-null
    Types<NodeId>::Array parents = parameters;
    if (lower != NULL_NODEID)
      parents.push_back(lower);

    if (upper != NULL_NODEID)
      parents.push_back(upper);

    return parents;
  }

  StochasticNode::StochasticNode(const DimArray::Ptr pDim,
                                 const Distribution::Ptr & pPrior,
                                 const Types<NodeId>::Array & parameters,
                                 NodeId lowerNodeId,
                                 NodeId upperNodeId) :
    Node(STOCHASTIC, pDim, mkParents(parameters, lowerNodeId, upperNodeId)),
        pPrior_(pPrior), lowerNodeId_(lowerNodeId), upperNodeId_(upperNodeId)
  {
    if (!pPrior_)
      throw LogicError("Can not create StochasticNode: Distribution::Ptr is NULL.");

    if (!pPrior_->CheckNParam(parameters.size()))
      throw LogicError("Can not create StochasticNode: invalid number of parameters.");

    if (IsBounded() && !pPrior_->CanBound())
      throw LogicError("Bounded node has non-boundable distribution");

  }
}
