//                                               -*- C++ -*-
/*! \file Node.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_NODE_HPP_
#define BIIPS_NODE_HPP_

#include "common/Common.hpp"

namespace Biips
{
  //! Type of node enumeration
  /*!
   * Nodes can be of three types.
   */
  enum NodeType // FIXME useless with visitor pattern ?
  {
    STOCHASTIC, //!< corresponds to StochasticNode type
    LOGICAL, //!< corresponds to LogicalNode type
    CONSTANT //!< corresponds to ConstantNode type
  };


  //! Node identifiers type
  /*!
   * Each Node in a Graph is associated a NodeId, i.e. a unique identifer
   * which is nothing but a positive integer.
   * It is used as index to access the Node in a Graph object or its
   * associated properties in arrays.
   */
  typedef Size NodeId;

  class NodeVisitor;
  class ConstNodeVisitor;


  //! Node abstract class
  /*!
   * Node class represents a node in a directed acyclic graph
   * defining a stochastic model.
   *
   * Each Node object have a dimension array member
   * and a parents array member.
   */
  class Node
  {
  protected:
    DimArray::Ptr pDim_;
    Types<NodeId>::Array parents_;

  public:
    typedef Node SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Types<SelfType>::Array Array;
    typedef Types<SelfType>::PtrArray PtrArray;
    typedef Types<SelfType>::IteratorPair IteratorPair;

    const DimArray & Dim() const { return *pDim_; }
    DimArray & Dim() { return *pDim_; }
    const DimArray::Ptr & DimPtr() const { return pDim_; }
    DimArray::Ptr & DimPtr() { return pDim_; }
    const Types<NodeId>::Array & Parents() const { return parents_; }
//    NodeId Parents(Size n) const { return parents_.at(n); }
    virtual NodeType GetType() const = 0; // FIXME useless with visitor pattern ?

    virtual void AcceptVisitor(NodeVisitor & vis) = 0;
    virtual void AcceptVisitor(ConstNodeVisitor & vis) const = 0;

    explicit Node(const DimArray::Ptr & pDim) : pDim_(pDim) {}
    Node(const DimArray::Ptr & pDim, const Types<NodeId>::Array & parents) : pDim_(pDim), parents_(parents) {}

    virtual ~Node() {};

  };

} /* namespace Biips */


#endif /* BIIPS_NODE_HPP_ */
