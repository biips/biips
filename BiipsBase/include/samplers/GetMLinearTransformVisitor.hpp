//                                               -*- C++ -*-
/*! \file GetMLinearTransformVisitor.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_GETMLINEARTRANSFORMVISITOR_HPP_
#define BIIPS_GETMLINEARTRANSFORMVISITOR_HPP_

#include "graph/NodeVisitor.hpp"
#include "common/Vector.hpp"
#include "common/Matrix.hpp"

namespace Biips
{

  class Graph;
  class NodeSampler;

  class GetMLinearTransformVisitor : public ConstNodeVisitor
  {
  protected:
    typedef GetMLinearTransformVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef DataType::StorageType StorageType;

    const Graph * pGraph_;
    NodeId myId_;
    NodeSampler * pNodeSampler_;
    Size dimNode_;
    Size dimObs_;
    Matrix A_;
    Vector b_;

  public:
    virtual void Visit(const ConstantNode & node) {}; // TODO throw exception

    virtual void Visit(const StochasticNode & node);

    virtual void Visit(const LogicalNode & node);

    const Matrix & GetA() { return A_; };
    const Vector & GetB() { return b_; };

    GetMLinearTransformVisitor(const Graph * pGraph, NodeId myId, NodeSampler * pSampleNodeVis, Size dimNode, Size dimObs);
  };


}

#endif /* BIIPS_GETMLINEARTRANSFORMVISITOR_HPP_ */
