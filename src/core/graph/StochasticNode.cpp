#include "graph/StochasticNode.hpp"
#include "graph/NodeVisitor.hpp"

namespace Biips
{

  static Types<NodeId>::Array mkParents(const Types<NodeId>::Array & parameters,
                                        NodeId lower,
                                        NodeId upper)
  {
    //Add bounds to vector of parents, if they are non-null
    Types<NodeId>::Array parents = parameters;
    if (lower != NULL_NODEID)
      parents.push_back(lower);

    if (upper != NULL_NODEID)
      parents.push_back(upper);

    return parents;
  }

  StochasticNode::StochasticNode(const DimArray::Ptr pDim,
                                 const Distribution::Ptr & pPrior,
                                 const Types<NodeId>::Array & parameters,
                                 NodeId lowerNodeId,
                                 NodeId upperNodeId) :
    Node(STOCHASTIC, pDim, mkParents(parameters, lowerNodeId, upperNodeId)),
        pPrior_(pPrior), lowerNodeId_(lowerNodeId), upperNodeId_(upperNodeId)
  {
    if (!pPrior_)
      throw LogicError("Can not create StochasticNode: Distribution::Ptr is NULL.");

    if (!pPrior_->CheckNParam(parameters.size()))
      throw LogicError("Can not create StochasticNode: invalid number of parameters.");

    if (IsBounded() && !pPrior_->CanBound())
      throw LogicError("Bounded node has non-boundable distribution");

  }
}
