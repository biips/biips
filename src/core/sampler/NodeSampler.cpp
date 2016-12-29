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

    // TODO double check the need of this
//    if (sampledFlagsMap()[nodeId_])
//      return;

    NumArray::Array params = getParamValues(nodeId_, graph_, *this);

    // allocate memory
    if (!nodeValuesMap()[nodeId_])
      nodeValuesMap()[nodeId_].reset(new ValArray(node.Dim().Length()));

    // FIXME
    // evaluate
    try
    {
      node.Eval(*nodeValuesMap()[nodeId_], params);
    }
    catch (RuntimeError & err)
    {
      throw NodeError(nodeId_, String(err.what()));
    }

    sampledFlagsMap()[nodeId_] = true;
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
    NumArray::Array param_values = getParamValues(nodeId_, graph_, *this);
    NumArray::Pair bound_values = getBoundValues(nodeId_, graph_, *this);

    // allocate memory
    if (!nodeValuesMap()[nodeId_])
      nodeValuesMap()[nodeId_].reset(new ValArray(node.Dim().Length()));

    if (!pRng_)
      throw LogicError("NodeSampler can not sample StochasticNode: Rng pointer is null.");

    // sample
    try
    {
      node.Sample(*nodeValuesMap()[nodeId_],
                  param_values,
                  bound_values,
                  *pRng_);
    }
    catch (RuntimeError & err)
    {
      throw NodeError(nodeId_, String(err.what()));
    }

    sampledFlagsMap()[nodeId_] = true;
    logIncrementalWeight_ = getLogLikelihood(graph_, nodeId_, *this);
  }

  void NodeSampler::Sample(NodeId nodeId)
  {
    graph_.VisitNode(nodeId, *this);
  }

  NodeSamplerFactory::Ptr NodeSamplerFactory::pFactoryInstance_(new NodeSamplerFactory());

}
