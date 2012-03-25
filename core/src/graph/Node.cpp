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
#include "graph/NodeVisitor.hpp"

namespace Biips
{

  Node::Node(NodeType type, const DimArray::Ptr & pDim) :
    nodeType_(type), pDim_(pDim)
  {
    if (!pDim)
      throw LogicError("Can not create node: DimArray::Ptr is NULL.");
  }

  Node::Node(NodeType type,
             const DimArray::Ptr & pDim,
             const Types<NodeId>::Array & parents) :
    nodeType_(type), pDim_(pDim), parents_(parents)
  {
    if (!pDim)
      throw LogicError("Can not create node: DimArray::Ptr is NULL.");
  }

}
