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
      throw LogicError(String("Incorrect number of parameters for function ") + name_);

    return checkParamDims(paramDims);
  };


  Bool Function::CheckParamValues(const MultiArray::Array & paramValues) const
  {
    Types<DimArray::Ptr>::Array param_dims(paramValues.size());
    for (Size i = 0; i<paramValues.size(); ++i)
      param_dims[i] = paramValues[i].DimPtr();

    if (!CheckParamDims(param_dims))
      throw LogicError(String("Non-conforming parameters dimensions for function ") + name_);

    return checkParamValues(paramValues);
  };


  DimArray Function::Dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckParamDims(paramDims))
      throw LogicError(String("Non-conforming parameters dimensions for function ") + name_);

    return dim(paramDims).Drop();
  }


  MultiArray Function::Eval(const MultiArray::Array & paramValues) const
  {
    if (!CheckParamValues(paramValues))
      throw LogicError(String("Invalid parameters values for function ") + name_);

    return eval(paramValues);
  }
}
