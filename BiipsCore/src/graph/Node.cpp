//                                               -*- C++ -*-
/*! \file Node.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "graph/Node.hpp"

namespace Biips
{

  Node::Node(const DimArray::Ptr & pDim)
    : pDim_(pDim)
  {
    if (!pDim)
      throw LogicError("Can not create node: DimArray::Ptr is NULL.");
  }

  Node::Node(const DimArray::Ptr & pDim, const Types<NodeId>::Array & parents)
    : pDim_(pDim), directParents_(parents)
  {
    if (!pDim)
      throw LogicError("Can not create node: DimArray::Ptr is NULL.");
  }
}
