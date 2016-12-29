#include "graph/NodeVisitor.hpp"

namespace Biips
{

  void NodeVisitor::Visit(ConstantNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit ConstantNode: node id not set.");

    visit(node);
  }

  void NodeVisitor::Visit(StochasticNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit StochasticNode: node id not set.");

    visit(node);
  }

  void NodeVisitor::Visit(LogicalNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit LogicalNode: node id not set.");

    visit(node);
  }

  void ConstNodeVisitor::Visit(const ConstantNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit ConstantNode: node id not set.");

    visit(node);
  }

  void ConstNodeVisitor::Visit(const StochasticNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit StochasticNode: node id not set.");

    visit(node);
  }

  void ConstNodeVisitor::Visit(const LogicalNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit LogicalNode: node id not set.");

    visit(node);
  }

}
