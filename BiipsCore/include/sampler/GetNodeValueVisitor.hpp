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

#include "graph/Graph.hpp"

namespace Biips
{
  class NodeSampler;
  class Monitor;

  MultiArray getNodeValue(NodeId nodeId, const Graph * pGraph, NodeSampler * pNodeSampler);
  MultiArray getNodeValue(NodeId nodeId, const Graph::Ptr & pGraph, const Monitor & monitor, Size particleIndex);

  MultiArray::Array getParamValues(NodeId nodeId, const Graph * pGraph, NodeSampler * pSampleNodeVis);
  MultiArray::Array getParamValues(NodeId nodeId, const Graph::Ptr & pGraph, const Monitor & monitor, Size particleIndex);
}


#endif /* BIIPS_GETNODEVALUEVISITOR_HPP_ */
