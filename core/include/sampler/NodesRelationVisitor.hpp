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

/*! \file NodesRelationVisitor.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_NODESRELATIONVISITOR_HPP_
#define BIIPS_NODESRELATIONVISITOR_HPP_

#include "graph/GraphTypes.hpp"

namespace Biips
{
  class Graph;

  //! Nodes relation function
  /*!
   * Answers the question: what is the relation between A and B?
   * For instance, we want to know, when computing nodeB,
   * which node values are known, i.e. have been computed before.
   * This is particularly useful when establishing conjugacy relations
   * between stochastic nodes.
   * @param nodeA we look at nodeA from nodeB point of view.
   * @param nodeB is expected to be a StochasticNode from the nodes sequence of the ForwardSampler object.
   * @param graph the Graph pointer to which we refer.
   * @return KNOWN, DEPENDING, or UNKNOWN
   * \see NodesRelationType
   */
  NodesRelationType nodesRelation(NodeId nodeA,
                                  NodeId nodeB,
                                  const Graph & graph);

  Bool anyUnknownParent(NodeId id, NodeId sampledId, const Graph & graph);

//  Types<NodesRelationType>::Array getParentnodesRelations(NodeId nodeId, const Graph & graph);
}

#endif /* BIIPS_NODESRELATIONVISITOR_HPP_ */
