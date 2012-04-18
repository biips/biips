//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
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

/*! \file Function.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "function/Function.hpp"

namespace Biips
{

  Bool Function::CheckParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckNParam(paramDims.size()))
      throw LogicError(String("Incorrect number of parameters for function ")
          + name_);

    return checkParamDims(paramDims);
  }

  Bool Function::CheckParamValues(const NumArray::Array & paramValues) const
  {
    //    Types<DimArray::Ptr>::Array param_dims(paramValues.size());
    //    for (Size i = 0; i<paramValues.size(); ++i)
    //      param_dims[i] = paramValues[i].DimPtr();
    //
    //    if (!CheckParamDims(param_dims))
    //      throw LogicError(String("Non-conforming parameters dimensions for function ") + name_);

    return checkParamValues(paramValues);
  }

  DimArray Function::Dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckParamDims(paramDims))
      throw RuntimeError(String("Non-conforming parameters dimensions for function ")
          + name_);

    return dim(paramDims).Drop();
  }

  void Function::Eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    if (!CheckParamValues(paramValues))
      throw LogicError(String("Invalid parameters values for function ")
          + name_);

    eval(values, paramValues);
  }
}
