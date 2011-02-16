//                                               -*- C++ -*-
/*! \file StochasticNode.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "graph/StochasticNode.hpp"
#include "graph/NodeVisitor.hpp"

namespace Biips
{

  void StochasticNode::AcceptVisitor(NodeVisitor & vis)
  {
    vis.Visit(*this);
  };

  void StochasticNode::AcceptVisitor(ConstNodeVisitor & vis) const
  {
    vis.Visit(*this);
  };

}
