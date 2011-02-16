//                                               -*- C++ -*-
/*! \file LogicalNode.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_LOGICALNODE_HPP_
#define BIIPS_LOGICALNODE_HPP_

#include "Node.hpp"
#include "function/Function.hpp"

namespace Biips
{

  //! LogicalNode concrete class
  /*!
   * LogicalNode objects are deterministic nodes in a DAG, defined by
   * a Function.
   *
   * This type of node can be defined by <- operator in BUGS language.
   */
  class LogicalNode : public Node
  {
  public:
    typedef LogicalNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Types<SelfType>::IteratorPair IteratorPair;

    virtual NodeType GetType() const { return LOGICAL; };
    virtual void AcceptVisitor(NodeVisitor & vis);
    virtual void AcceptVisitor(ConstNodeVisitor & vis) const;

    virtual const String & FuncName() const = 0;
    virtual DataType Eval(const DataType::Array & paramValues) const = 0;

    LogicalNode(const DimArray::Ptr pDim, const Types<NodeId>::Array & parameters) : Node(pDim, parameters) {}

    virtual ~LogicalNode() {};
  };

}

#endif /* BIIPS_LOGICALNODE_HPP_ */
