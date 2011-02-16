//                                               -*- C++ -*-
/*! \file LogicalNode.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "graph/LogicalNode.hpp"
#include "graph/NodeVisitor.hpp"

namespace Biips
{

  void LogicalNode::AcceptVisitor(NodeVisitor & vis)
  {
    vis.Visit(*this);
  };

  void LogicalNode::AcceptVisitor(ConstNodeVisitor & vis) const
  {
    vis.Visit(*this);
  };

}
