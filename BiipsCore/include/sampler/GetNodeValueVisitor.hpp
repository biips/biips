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

namespace Biips
{
  class Graph;
  class NodeSampler;
  class Monitor;

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
                                 const Monitor & monitor,
                                 Size particleIndex);

  NumArray::Pair getBoundValues(NodeId nodeId,
                                const Graph & graph,
                                NodeSampler & nodeSampler);
  NumArray::Pair getBoundValues(NodeId nodeId,
                                const Graph & graph,
                                const Monitor & monitor,
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
                        const Monitor & monitor,
                        Size particleIndex);

  Bool isSupportFixed(NodeId nodeId, const Graph & graph);
}

#endif /* BIIPS_GETNODEVALUEVISITOR_HPP_ */
