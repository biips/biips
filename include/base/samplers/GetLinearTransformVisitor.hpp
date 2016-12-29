#ifndef BIIPS_GETLINEARTRANSFORMVISITOR_HPP_
#define BIIPS_GETLINEARTRANSFORMVISITOR_HPP_

#include "graph/NodeVisitor.hpp"

namespace Biips
{

  class Graph;
  class NodeSampler;

  class GetLinearTransformVisitor : public ConstNodeVisitor
  {
  protected:
    typedef GetLinearTransformVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph & graph_;
    NodeId myId_;
    NodeSampler & nodeSampler_;
    Scalar a_;
    Scalar b_;

    virtual void visit(const ConstantNode & node)
    {
      throw LogicError("ConstantNode can not be visited by GetLinearTransformVisitor.");
    }

    virtual void visit(const StochasticNode & node);

    virtual void visit(const LogicalNode & node);

  public:
    Scalar GetA() const { return a_; }
    Scalar GetB() const { return b_; }

    GetLinearTransformVisitor(const Graph & graph, NodeId myId, NodeSampler & nodeSampler);

  };
}

#endif /* BIIPS_GETLINEARTRANSFORMVISITOR_HPP_ */
