//                                               -*- C++ -*-
/*! \file LogLikeVisitor.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_LOGLIKEVISITOR_HPP_
#define BIIPS_LOGLIKEVISITOR_HPP_

#include "graph/NodeVisitor.hpp"

namespace Biips
{

  class Graph;
  class NodeSampler;

  class LogLikeVisitor : public ConstNodeVisitor
  {
  protected:
    typedef LogLikeVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph * pGraph_;
    NodeId myId_;
    NodeSampler * pSampleNodeVis_;
    Scalar logLikelihood_;

  public:
    virtual void Visit(const ConstantNode & node) {}; // TODO throw exception
    virtual void Visit(const LogicalNode & node) {}; // TODO throw exception

    virtual void Visit(const StochasticNode & node);

    Scalar Value() const { return logLikelihood_; };

    LogLikeVisitor(const Graph * pGraph, NodeId myId, NodeSampler * pSampleNodeVis);
  };
}

#endif /* BIIPS_LOGLIKEVISITOR_HPP_ */
