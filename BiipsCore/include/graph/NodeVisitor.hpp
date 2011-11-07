//                                               -*- C++ -*-
/*! \file NodeVisitor.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

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

    NodeVisitor() : nodeId_(NULL_NODEID) {}

    virtual void visit(ConstantNode & node) = 0;
    virtual void visit(StochasticNode & node) = 0;
    virtual void visit(LogicalNode & node) = 0;

  public:
    void Visit(ConstantNode & node);
    void Visit(StochasticNode & node);
    void Visit(LogicalNode & node);

    void SetNodeId(NodeId nodeId) { nodeId_ = nodeId; }

    virtual ~NodeVisitor() {}
  };


  class ConstNodeVisitor
  {
  protected:
    NodeId nodeId_;

    ConstNodeVisitor() : nodeId_(NULL_NODEID) {}

    virtual void visit(const ConstantNode & node) = 0;
    virtual void visit(const StochasticNode & node) = 0;
    virtual void visit(const LogicalNode & node) = 0;

  public:
    void Visit(const ConstantNode & node);
    void Visit(const StochasticNode & node);
    void Visit(const LogicalNode & node);

    void SetNodeId(NodeId nodeId) { nodeId_ = nodeId; }

    virtual ~ConstNodeVisitor() {}
  };



  class ConstStochasticNodeVisitor : public ConstNodeVisitor
  {
  protected:
    ConstStochasticNodeVisitor() {}

    virtual void visit(const ConstantNode & node) { throw LogicError("ConstantNode can not be visited by StochasticNodeVisitor."); }
    virtual void visit(const StochasticNode & node) = 0;
    virtual void visit(const LogicalNode & node) { throw LogicError("LogicalNode can not be visited by StochasticNodeVisitor."); };

  public:
    virtual ~ConstStochasticNodeVisitor() {}
  };
}


#endif /* BIIPS_NODEVISITOR_HPP_ */
