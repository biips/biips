#include "graph/AggNode.hpp"

namespace Biips
{

  const String AggNode::NAME_ = "aggregate";

  AggNode::AggNode(const DimArray::Ptr pDim,
                   const Types<NodeId>::Array & parameters,
                   const Types<Size>::Array & offsets) :
    LogicalNode(pDim, parameters), offsets_(offsets)
  {
    Size len = pDim->Length();
    if (len != parameters.size() || len != offsets.size())
      throw LogicError("Can not create aggregate node: dimensions mismatch.");
  }

  void AggNode::Eval(ValArray & values, const NumArray::Array & paramValues) const
  // TODO checks
  {
    for (Size i = 0; i < Dim().Length(); ++i)
      values[i] = paramValues[i].Values()[offsets_[i]];
  }

}
