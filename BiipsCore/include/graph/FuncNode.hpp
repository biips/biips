//                                               -*- C++ -*-
/*! \file FuncNode.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_FUNCNODE_HPP_
#define BIIPS_FUNCNODE_HPP_

#include "graph/LogicalNode.hpp"

namespace Biips
{

  class FuncNode : public LogicalNode
  {
  public:
    typedef FuncNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Types<SelfType>::IteratorPair IteratorPair;

  protected:
    Function::Ptr pFunc_;

  public:
    const Function::Ptr & FuncPtr() const { return pFunc_; };
    virtual const String & FuncName() const { return pFunc_->Name(); };
    virtual MultiArray Eval(const MultiArray::Array & paramValues) const { return pFunc_->Eval(paramValues); };
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const { return pFunc_->IsScale(scaleMask, knownMask); }
    virtual Bool IsLinear(const Flags & linearMask, const Flags & knownMask) const { return pFunc_->IsLinear(linearMask, knownMask); }

    FuncNode(const DimArray::Ptr pDim, const Function::Ptr & pFunc,
        const Types<NodeId>::Array & parameters);
  };

}

#endif /* BIIPS_FUNCNODE_HPP_ */
