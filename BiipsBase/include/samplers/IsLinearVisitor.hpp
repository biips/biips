//                                               -*- C++ -*-
/*! \file IsLinearVisitor.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_ISLINEARVISITOR_HPP_
#define BIIPS_ISLINEARVISITOR_HPP_

#include "graph/NodeVisitor.hpp"

namespace Biips
{
  
  class Graph;


  class IsLinearVisitor : public ConstNodeVisitor
  {
  public:
    typedef IsLinearVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    enum LinearFuncType
    {
      ADD,
      MULTIPLY,
      MAT_MULT,
      SUBSTRACT,
      DIVIDE,
      NEG
    };

  protected:
    const Graph * pGraph_;
    NodeId myId_;
    Bool ans_;

  public:
    static const std::map<String, LinearFuncType> & LinearFuncMap();

    virtual void Visit(const ConstantNode & node) { ans_ = false; };

    virtual void Visit(const StochasticNode & node);

    virtual void Visit(const LogicalNode & node);

    Bool IsLinear() const { return ans_; };

    IsLinearVisitor(const Graph * pGraph, NodeId myId)
    : pGraph_(pGraph), myId_(myId), ans_(false) {}
  };


  // nodeB is expected to be a StochasticNode from the nodeSequence
  Bool isLinear(NodeId nodeA, NodeId nodeB, const Graph * pGraph);

}

#endif /* BIIPS_CHECKLINEARITYVISITOR_HPP_ */
