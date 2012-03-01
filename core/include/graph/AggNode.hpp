//                                               -*- C++ -*-
/*! \file AggNode.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_AGGNODE_HPP_
#define BIIPS_AGGNODE_HPP_

#include "graph/LogicalNode.hpp"

namespace Biips
{

  class AggNode: public LogicalNode
  {
  public:
    typedef AggNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    Types<Size>::Array offsets_;

    static const String NAME_;

  public:
    static const String & Name()
    {
      return NAME_;
    }

    virtual const String & FuncName() const
    {
      return Name();
    }
    virtual void
    Eval(ValArray & values, const NumArray::Array & paramValues) const;
    virtual Bool IsFunction() const
    {
      return false;
    }
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const
    {
      return false;
    }
    virtual Bool IsLinear(const Flags & linearMask, const Flags & knownMask) const
    {
      return false;
    }

    AggNode(const DimArray::Ptr pDim,
            const Types<NodeId>::Array & parameters,
            const Types<Size>::Array & offsets);
  };

}

#endif /* BIIPS_AGGNODE_HPP_ */
