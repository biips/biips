//                                               -*- C++ -*-
/*! \file ConstantNode.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "graph/ConstantNode.hpp"
#include "graph/NodeVisitor.hpp"

namespace Biips
{

  void ConstantNode::AcceptVisitor(NodeVisitor & vis)
  {
    vis.Visit(*this);
  };

  void ConstantNode::AcceptVisitor(ConstNodeVisitor & vis) const
  {
    vis.Visit(*this);
  };

}
