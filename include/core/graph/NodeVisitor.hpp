#ifndef BIIPS_NODEVISITOR_HPP_
#define BIIPS_NODEVISITOR_HPP_

#include "GraphTypes.hpp"

namespace Biips
{

  class ConstantNode;
  class StochasticNode;
  class LogicalNode;

  class NodeVisitor
  {
  protected:
    NodeId nodeId_;

    NodeVisitor() :
      nodeId_(NULL_NODEID)
    {
    }

    virtual void visit(ConstantNode & node)
    {
      throw LogicError("NodeVisitor: can not visit ConstantNode.");
    }
    virtual void visit(StochasticNode & node)
    {
      throw LogicError("NodeVisitor: can not visit StochasticNode.");
    }
    virtual void visit(LogicalNode & node)
    {
      throw LogicError("NodeVisitor: can not visit LogicalNode.");
    }

  public:
    void Visit(ConstantNode & node);
    void Visit(StochasticNode & node);
    void Visit(LogicalNode & node);

    void SetNodeId(NodeId nodeId)
    {
      nodeId_ = nodeId;
    }

    virtual ~NodeVisitor()
    {
    }
  };

  class ConstNodeVisitor
  {
  protected:
    NodeId nodeId_;

    ConstNodeVisitor() :
      nodeId_(NULL_NODEID)
    {
    }

    virtual void visit(const ConstantNode & node)
    {
      throw LogicError("ConstNodeVisitor: can not visit ConstantNode.");
    }
    virtual void visit(const StochasticNode & node)
    {
      throw LogicError("ConstNodeVisitor: can not visit StochasticNode.");
    }
    virtual void visit(const LogicalNode & node)
    {
      throw LogicError("ConstNodeVisitor: can not visit LogicalNode.");
    }

  public:
    void Visit(const ConstantNode & node);
    void Visit(const StochasticNode & node);
    void Visit(const LogicalNode & node);

    void SetNodeId(NodeId nodeId)
    {
      nodeId_ = nodeId;
    }

    virtual ~ConstNodeVisitor()
    {
    }
  };

}

#endif /* BIIPS_NODEVISITOR_HPP_ */
