//                                               -*- C++ -*-
/*! \file FuncNode.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "graph/FuncNode.hpp"

namespace Biips
{

  FuncNode::FuncNode(const DimArray::Ptr pDim, const Function::Ptr & pFunc,
      const Types<NodeId>::Array & parameters)
    : LogicalNode(pDim, parameters), pFunc_(pFunc)
  {
    if (!pFunc_)
      throw LogicError("Can not create LogicalNode: Function::Ptr is NULL.");

    if (!pFunc_->CheckNParam(parameters.size()))
      throw LogicError("Can not create LogicalNode: invalid number of parameters.");
  }
}
