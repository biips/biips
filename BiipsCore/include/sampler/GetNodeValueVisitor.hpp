//                                               -*- C++ -*-
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
