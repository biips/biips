//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file FuncNode.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_FUNCNODE_HPP_
#define BIIPS_FUNCNODE_HPP_

#include "graph/LogicalNode.hpp"

namespace Biips
{

  class FuncNode: public LogicalNode
  {
  public:
    typedef FuncNode SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Types<SelfType>::IteratorPair IteratorPair;

  protected:
    Function::Ptr pFunc_;

  public:
    const Function::Ptr & FuncPtr() const
    {
      return pFunc_;
    }
    virtual const String & FuncName() const
    {
      return pFunc_->Name();
    }
    virtual void Eval(ValArray & values, const NumArray::Array & paramValues) const
    {
      pFunc_->Eval(values, paramValues);
    }
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const
    {
      return pFunc_->IsScale(scaleMask, knownMask);
    }
    virtual Bool IsLinear(const Flags & linearMask, const Flags & knownMask) const
    {
      return pFunc_->IsLinear(linearMask, knownMask);
    }

    FuncNode(const DimArray::Ptr pDim,
             const Function::Ptr & pFunc,
             const Types<NodeId>::Array & parameters);
  };

}

#endif /* BIIPS_FUNCNODE_HPP_ */
