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
#include "common/NumArray.hpp"
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
                                 const Types<Monitor*>::Array & monitors,
                                 Size particleIndex);

  NumArray::Pair getBoundValues(NodeId nodeId,
                                const Graph & graph,
                                NodeSampler & nodeSampler);
  NumArray::Pair getBoundValues(NodeId nodeId,
                                const Graph & graph,
                                const Types<Monitor*>::Pair & monitors,
                                Size particleIndex);

  void getFixedSupportValues(ValArray & lower,
                        ValArray & upper,
                        NodeId nodeId,
                        const Graph & graph);
//  void getSupportValues(ValArray & lower,
//                        ValArray & upper,
//                        NodeId nodeId,
//                        const Graph & graph,
//                        const Types<Monitor*>::Array & paramMonitors,
//                        const Types<Monitor*>::Pair & boundMonitors,
//                        Size particleIndex);

  Bool isSupportFixed(NodeId nodeId, const Graph & graph);
}

#endif /* BIIPS_GETNODEVALUEVISITOR_HPP_ */
