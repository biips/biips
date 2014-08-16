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

/*! \file AggNode.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "graph/AggNode.hpp"

namespace Biips
{

  const String AggNode::NAME_ = "aggregate";

  AggNode::AggNode(const DimArray::Ptr pDim,
                   const Types<NodeId>::Array & parameters,
                   const Types<Size>::Array & offsets) :
    LogicalNode(pDim, parameters), offsets_(offsets)
  {
    Size len = pDim->Length();
    if (len != parameters.size() || len != offsets.size())
      throw LogicError("Can not create aggregate node: dimensions mismatch.");
  }

  void AggNode::Eval(ValArray & values, const NumArray::Array & paramValues) const
  // TODO checks
  {
    for (Size i = 0; i < Dim().Length(); ++i)
      values[i] = paramValues[i].Values()[offsets_[i]];
  }

}
