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
