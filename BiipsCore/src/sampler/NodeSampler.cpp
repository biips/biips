//                                               -*- C++ -*-
/*! \file NodeSampler.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/NodeSampler.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/LogLikeVisitor.hpp"
#include "graph/ConstantNode.hpp"
#include "graph/LogicalNode.hpp"
#include "graph/StochasticNode.hpp"

namespace Biips
{

  const String NodeSampler::NAME_ = "Prior";


  void NodeSampler::visit(const LogicalNode & node)
  {
    if (!membersSet_)
      throw LogicError("NodeSampler can not visit LogicalNode: members not set.");

    if (sampledFlagsMap_[nodeId_])
      return;

    MultiArray::Array params = getParamValues(nodeId_, graph_, *this);
    nodeValuesMap_[nodeId_] = node.Eval(params).ValuesPtr();

    sampledFlagsMap_[nodeId_] = true;
  }


  void NodeSampler::visit(const StochasticNode & node)
  {
    if (!membersSet_)
      throw LogicError("NodeSampler can not visit StochasticNode: members not set.");

    // FIXME
//    try
//    {
      sample(node);
//    }
//    catch (RuntimeError & err)
//    {
//      throw NodeError(nodeId_, String("Failure to sample stochastic node.\n") + err.what());
//    }
  }


  void NodeSampler::sample(const StochasticNode & node)
  {
    MultiArray::Array param_values = getParamValues(nodeId_, graph_, *this);
    MultiArray::Pair bound_values = getBoundValues(nodeId_, graph_, *this);
    nodeValuesMap_[nodeId_] = node.Sample(param_values, bound_values, *pRng_).ValuesPtr();
    sampledFlagsMap_[nodeId_] = true;
    logIncrementalWeight_ = getLogLikelihood(graph_, nodeId_, *this);
  }


  void NodeSampler::Sample(NodeId nodeId)
  {
    graph_.VisitNode(nodeId, *this);
  };


  NodeSamplerFactory::Ptr NodeSamplerFactory::pFactoryInstance_(new NodeSamplerFactory());

}
