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

/*! \file   ConstantFactory.cpp
 * \brief   
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "compiler/ConstantFactory.hpp"

namespace Biips
{

  bool lt(DimArray const &dim1, DimArray const &dim2)
  {
    if (dim1.size() == dim2.size())
    {
      for (Size i = 0; i < dim1.size(); ++i)
      {
        if (dim1[i] == dim2[i])
          continue;
        else
          return dim1[i] < dim2[i];
      }
    }
    else
      return dim1.size() < dim2.size();

    return false;
  }

  /* Comparison function for ValArray of equal length */
  bool lt(ValArray const &value1, ValArray const &value2)
  {
    for (Size i = 0; i < value1.size(); ++i)
    {
      if (lt(value1[i], value2[i]))
        return true;
      else if (lt(value2[i], value1[i]))
        return false;
    }
    return false;
  }

  bool ltMultiArray::operator()(const MultiArray & arg1,
                                const MultiArray & arg2) const
  {
    if (lt(arg1.Dim(), arg2.Dim()))
      return true;
    else if (lt(arg2.Dim(), arg1.Dim()))
      return false;
    else
      return lt(arg2.Values(), arg1.Values());
  }

  NodeId ConstantFactory::GetNode(Scalar value)
  {
    std::map<Scalar, NodeId, ltScalar>::const_iterator it =
        constMap_.find(value);

    if (it != constMap_.end())
      return it->second;

    ValArray::Ptr pval(new ValArray(1, value));
    NodeId id = NewNode(P_SCALAR_DIM, pval);

    constMap_[value] = id;

    return id;
  }

  NodeId ConstantFactory::GetNode(const DimArray::Ptr & pDim,
                                  const ValArray::Ptr & pVal)
  {
    MultiArray marray(pDim, pVal);

    std::map<MultiArray, NodeId>::const_iterator it = mvConstMap_.find(marray);

    if (it != mvConstMap_.end())
      return it->second;

    // Create a new constant node
    NodeId id = NewNode(pDim, pVal);

    mvConstMap_[marray] = id;

    return id;
  }

  NodeId ConstantFactory::NewNode(const DimArray::Ptr & pDim,
                                  const ValArray::Ptr & pVal)
  {
    return graph_.AddConstantNode(pDim, pVal);
  }

} /* namespace Biips */
