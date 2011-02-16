//                                               -*- C++ -*-
/*! \file GetLinearTransformVisitor.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

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

    const Graph * pGraph_;
    NodeId myId_;
    NodeSampler * pNodeSampler_;
    Scalar a_;
    Scalar b_;

  public:
    virtual void
    Visit(const ConstantNode & node)
    {
    }
    ; // TODO throw exception

    virtual void
    Visit(const StochasticNode & node);

    virtual void
    Visit(const LogicalNode & node);

    Scalar
    GetA() const
    {
      return a_;
    }
    ;
    Scalar
    GetB() const
    {
      return b_;
    }
    ;

    GetLinearTransformVisitor(const Graph * pGraph, NodeId myId,
        NodeSampler * pNodeSampler);

  };
}

#endif /* BIIPS_GETLINEARTRANSFORMVISITOR_HPP_ */
