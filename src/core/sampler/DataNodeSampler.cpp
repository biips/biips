#include "sampler/DataNodeSampler.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "graph/StochasticNode.hpp"

namespace Biips
{

  const String DataNodeSampler::NAME_ = "Data sampler";

  void DataNodeSampler::sample(const StochasticNode & node)
  {
    NumArray::Array param_values = getParamValues(nodeId_, graph_, *this);
    NumArray::Pair bound_values = getBoundValues(nodeId_, graph_, *this);

    // allocate memory
    if (!nodeValuesMap()[nodeId_])
      nodeValuesMap()[nodeId_].reset(new ValArray(node.Dim().Length()));

    if (!pRng_)
      throw LogicError(
          "DataNodeSampler can not sample StochasticNode: Rng pointer is null.");

    // sample
    try
    {
      node.Sample(*nodeValuesMap()[nodeId_], param_values, bound_values,
                  *pRng_);
    }
    catch (RuntimeError & err)
    {
      throw NodeError(nodeId_, String(err.what()));
    }

    sampledFlagsMap()[nodeId_] = true;
  }
} /* namespace Biips */
