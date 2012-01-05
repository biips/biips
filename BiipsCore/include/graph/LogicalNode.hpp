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

  class LogicalNode: public Node
  {
  public:
    typedef LogicalNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Types<SelfType>::IteratorPair IteratorPair;

    virtual const String & FuncName() const = 0;
    virtual void
        Eval(ValArray & values, const NumArray::Array & paramValues) const = 0;
    virtual Bool IsFunction() const
    {
      return true;
    }
    virtual Bool
        IsScale(const Flags & scaleMask, const Flags & knownMask) const = 0;
    virtual Bool
        IsLinear(const Flags & linearMask, const Flags & knownMask) const = 0;

    LogicalNode(const DimArray::Ptr pDim,
                const Types<NodeId>::Array & parameters) :
      Node(LOGICAL, pDim, parameters)
    {
    }

    virtual ~LogicalNode()
    {
    }
  };

}

#endif /* BIIPS_LOGICALNODE_HPP_ */
