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

  class GetMLinearTransformVisitor: public ConstNodeVisitor
  {
  protected:
    typedef GetMLinearTransformVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef MultiArray::StorageType StorageType;

    const Graph & graph_;
    NodeId myId_;
    NodeSampler & nodeSampler_;
    Size dimNode_;
    Size dimObs_;
    Matrix A_;
    Vector b_;

    virtual void visit(const ConstantNode & node)
    {
      throw LogicError("ConstantNode can not be visited by GetLinearTransformVisitor.");
    }

    virtual void visit(const StochasticNode & node);

    virtual void visit(const LogicalNode & node);

  public:
    const Matrix & GetA()
    {
      return A_;
    }
    const Vector & GetB()
    {
      return b_;
    }

    GetMLinearTransformVisitor(const Graph & graph,
                               NodeId myId,
                               NodeSampler & nodeSampler,
                               Size dimNode,
                               Size dimObs);
  };

}

#endif /* BIIPS_GETMLINEARTRANSFORMVISITOR_HPP_ */
