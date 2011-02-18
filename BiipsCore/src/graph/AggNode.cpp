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

  DataType AggNode::Eval(const DataType::Array & paramValues) const
  // TODO checks
  {
    DataType ans(pDim_);
    for (Size i = 0; i < ans.Length(); ++i)
    {
      ans.Values()[i] = paramValues[i].Values()[offsets_[i]];
    }
    return ans;
  }

}
