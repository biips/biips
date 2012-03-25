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

  class NodeVisitor;
  class ConstNodeVisitor;

  class Node
  {
  protected:
    const NodeType nodeType_;
    DimArray::Ptr pDim_;
    Types<NodeId>::Array parents_;

  public:
    typedef Node SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Types<SelfType>::Array Array;
    typedef Types<SelfType>::PtrArray PtrArray;
    typedef Types<SelfType>::IteratorPair IteratorPair;

    const DimArray & Dim() const
    {
      return *pDim_;
    }
    DimArray & Dim()
    {
      return *pDim_;
    }
    const DimArray::Ptr & DimPtr() const
    {
      return pDim_;
    }
    DimArray::Ptr & DimPtr()
    {
      return pDim_;
    }
    const Types<NodeId>::Array & Parents() const
    {
      return parents_;
    }
    NodeType GetType() const
    {
      return nodeType_;
    }

    explicit Node(NodeType type, const DimArray::Ptr & pDim);
    Node(NodeType type,
         const DimArray::Ptr & pDim,
         const Types<NodeId>::Array & parents);

    virtual ~Node()
    {
    }

  };

} /* namespace Biips */

#endif /* BIIPS_NODE_HPP_ */
