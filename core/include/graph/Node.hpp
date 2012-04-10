//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
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
