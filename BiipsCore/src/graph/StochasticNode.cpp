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


  static Types<NodeId>::Array mkParents(const Types<NodeId>::Array & parameters,
                                        NodeId lower, NodeId upper)
  {
      //Add bounds to vector of parents, if they are non-null
      Types<NodeId>::Array parents = parameters;
      if (lower != NULL_NODEID)
          parents.push_back(lower);

      if (upper != NULL_NODEID)
          parents.push_back(upper);

      return parents;
  }


  StochasticNode::StochasticNode(const DimArray::Ptr pDim, const Distribution::Ptr & pPrior, const Types<NodeId>::Array & parameters, NodeId lowerNodeId, NodeId upperNodeId)
    : Node(pDim, mkParents(parameters, lowerNodeId, upperNodeId)), pPrior_(pPrior), lowerNodeId_(lowerNodeId), upperNodeId_(upperNodeId)
  {
    if (!pPrior_)
      throw LogicError("Can not create StochasticNode: Distribution::Ptr is NULL.");

    if (!pPrior_->CheckNParam(parameters.size()))
      throw LogicError("Can not create StochasticNode: invalid number of parameters.");

    if (IsBounded() && !pPrior_->CanBound())
      throw LogicError("Bounded node has non-boundable distribution");

  }
}
