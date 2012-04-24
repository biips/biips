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
