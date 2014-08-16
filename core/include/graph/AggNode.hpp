//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file AggNode.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
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
