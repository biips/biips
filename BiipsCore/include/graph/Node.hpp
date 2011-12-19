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

  enum NodeType // FIXME useless with visitor pattern ?
  {
    STOCHASTIC,
    LOGICAL,
    CONSTANT
  };


  class NodeVisitor;
  class ConstNodeVisitor;


  class Node
  {
  protected:
    DimArray::Ptr pDim_;
    Types<NodeId>::Array directParents_;

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
    const Types<NodeId>::Array & Parents() const { return directParents_; }
//    NodeId Parents(Size n) const { return parents_.at(n); }
    virtual NodeType GetType() const = 0; // FIXME useless with visitor pattern ?

    virtual void AcceptVisitor(NodeVisitor & vis) = 0;
    virtual void AcceptVisitor(ConstNodeVisitor & vis) const = 0;

    explicit Node(const DimArray::Ptr & pDim);
    Node(const DimArray::Ptr & pDim, const Types<NodeId>::Array & parents);

    virtual ~Node() {};

  };

} /* namespace Biips */


#endif /* BIIPS_NODE_HPP_ */
