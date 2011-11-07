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

  MultiArray getNodeValue(NodeId nodeId, const Graph & graph, NodeSampler & nodeSampler);
  MultiArray getNodeValue(NodeId nodeId, const Graph & graph, const Monitor & monitor, Size particleIndex);

//  Bool isBounded(NodeId nodeId, const Graph & graph);

  MultiArray::Array getParamValues(NodeId nodeId, const Graph & graph, NodeSampler & nodeSampler);
  MultiArray::Array getParamValues(NodeId nodeId, const Graph & graph, const Monitor & monitor, Size particleIndex);

  MultiArray::Pair getBoundValues(NodeId nodeId, const Graph & graph, NodeSampler & nodeSampler);
  MultiArray::Pair getBoundValues(NodeId nodeId, const Graph & graph, const Monitor & monitor, Size particleIndex);

  MultiArray::Pair getSupportValues(NodeId nodeId, const Graph & graph, NodeSampler & nodeSampler);
  MultiArray::Pair getSupportValues(NodeId nodeId, const Graph & graph, const Monitor & monitor, Size particleIndex);

  Bool isSupportFixed(NodeId nodeId, const Graph & graph);
}


#endif /* BIIPS_GETNODEVALUEVISITOR_HPP_ */
