//                                               -*- C++ -*-
/*! \file AggNode.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "graph/AggNode.hpp"


namespace Biips
{

  const String AggNode::NAME_ = "aggregate";


  AggNode::AggNode(const DimArray::Ptr pDim, const Types<NodeId>::Array & parameters, const Types<Size>::Array & offsets)
    : LogicalNode(pDim, parameters), offsets_(offsets)
  {
    Size len = pDim->Length();
    if (len != parameters.size() || len != offsets.size())
      throw LogicError("Can not create aggregate node: dimensions mismatch.");
  }

  MultiArray AggNode::Eval(const MultiArray::Array & paramValues) const
  // TODO checks
  {
    MultiArray ans(pDim_);
    for (Size i = 0; i < ans.Length(); ++i)
    {
      ans.Values()[i] = paramValues[i].Values()[offsets_[i]];
    }
    return ans;
  }

}
