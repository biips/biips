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

/*! \file GetNodeValueVisitor.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_GETNODEVALUEVISITOR_HPP_
#define BIIPS_GETNODEVALUEVISITOR_HPP_

#include "common/Types.hpp"
#include "common/MultiArray.hpp"
#include "model/Monitor.hpp"

namespace Biips
{
  class Graph;
  class NodeSampler;

  NumArray getNodeValue(NodeId nodeId,
                        const Graph & graph,
                        NodeSampler & nodeSampler);
  NumArray getNodeValue(NodeId nodeId,
                        const Graph & graph,
                        const Monitor & monitor,
                        Size particleIndex);

  //  Bool isBounded(NodeId nodeId, const Graph & graph);

  NumArray::Array getParamValues(NodeId nodeId,
                                 const Graph & graph,
                                 NodeSampler & nodeSampler);
  NumArray::Array getParamValues(NodeId nodeId,
                                 const Graph & graph,
                                 const Types<Monitor::Ptr>::Array & monitors,
                                 Size particleIndex);

  NumArray::Pair getBoundValues(NodeId nodeId,
                                const Graph & graph,
                                NodeSampler & nodeSampler);
  NumArray::Pair getBoundValues(NodeId nodeId,
                                const Graph & graph,
                                const Types<Monitor::Ptr>::Pair & monitors,
                                Size particleIndex);

  void getSupportValues(ValArray & lower,
                        ValArray & upper,
                        NodeId nodeId,
                        const Graph & graph,
                        NodeSampler & nodeSampler);
  void getSupportValues(ValArray & lower,
                        ValArray & upper,
                        NodeId nodeId,
                        const Graph & graph,
                        const Types<Monitor::Ptr>::Array & paramMonitors,
                        const Types<Monitor::Ptr>::Pair & boundMonitors,
                        Size particleIndex);

  Bool isSupportFixed(NodeId nodeId, const Graph & graph);
}

#endif /* BIIPS_GETNODEVALUEVISITOR_HPP_ */
