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
    Bool nodeIdDefined_;

    NodeVisitor() : nodeId_(0), nodeIdDefined_(false)  {};

  public:
    virtual void Visit(ConstantNode & node) = 0;
    virtual void Visit(StochasticNode & node) = 0;
    virtual void Visit(LogicalNode & node) = 0;

    void SetNodeId(NodeId nodeId) { nodeId_ = nodeId; nodeIdDefined_ = true; };

    virtual ~NodeVisitor() {};
  };


  class ConstNodeVisitor
  {
  protected:
    NodeId nodeId_;
    Bool nodeIdDefined_;

    ConstNodeVisitor() : nodeId_(0), nodeIdDefined_(false)  {};

  public:
    virtual void Visit(const ConstantNode & node) = 0;
    virtual void Visit(const StochasticNode & node) = 0;
    virtual void Visit(const LogicalNode & node) = 0;

    void SetNodeId(NodeId nodeId) { nodeId_ = nodeId; nodeIdDefined_ = true; };

    virtual ~ConstNodeVisitor() {};
  };

}


#endif /* BIIPS_NODEVISITOR_HPP_ */
