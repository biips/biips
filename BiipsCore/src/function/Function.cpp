//                                               -*- C++ -*-
/*! \file Function.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "function/Function.hpp"

namespace Biips
{

  Bool Function::CheckParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckNParam(paramDims.size()))
      throw LogicError(String("Incorrect number of parameters for ") + name_);

    return checkParamDims(paramDims);
  };


  DimArray Function::Dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckParamDims(paramDims))
      throw LogicError(String("Non-conforming parameters dimensions for function ") + name_);

    return dim(paramDims).Drop();
  }
}
