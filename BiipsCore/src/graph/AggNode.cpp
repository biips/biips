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
