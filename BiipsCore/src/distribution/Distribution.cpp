//                                               -*- C++ -*-
/*! \file Distribution.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distribution/Distribution.hpp"

namespace Biips
{

  Bool Distribution::CheckParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckNParam(paramDims.size()))
      throw LogicError(String("Incorrect number of parameters for distribution ") + name_);

    return checkParamDims(paramDims);
  };


  Bool Distribution::CheckParamValues(const MultiArray::Array & paramValues) const
  {
//    Types<DimArray::Ptr>::Array param_dims(paramValues.size());
//    for (Size i = 0; i<paramValues.size(); ++i)
//      param_dims[i] = paramValues[i].DimPtr();
//
//    if (!CheckParamDims(param_dims))
//      throw LogicError(String("Non-conforming parameters dimensions for distribution ") + name_);

    return checkParamValues(paramValues);
  }


  Bool Distribution::CheckDensityParamValues(const MultiArray & x, const MultiArray::Array & paramValues) const
  {
    return CheckParamValues(paramValues);
  }


  DimArray Distribution::Dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckParamDims(paramDims))
      throw RuntimeError(String("Non-conforming parameters dimensions for distribution ") + name_);

    return dim(paramDims).Drop();
  }

  MultiArray Distribution::Sample(const MultiArray::Array & paramValues,
      const MultiArray::Pair & boundValues, Rng & rng) const
  {
    if (!CheckParamValues(paramValues))
      throw RuntimeError(String("Invalid parameters values in Sample method for distribution ") + name_);

    return sample(paramValues, boundValues, rng);
  }

  Scalar Distribution::LogDensity(const MultiArray & x, const MultiArray::Array & paramValues,
      const MultiArray::Pair & boundValues) const
  {
    if (!CheckDensityParamValues(x, paramValues))
      throw RuntimeError(String("Invalid parameters values in LogDensity method for distribution ") + name_);

    return logDensity(x, paramValues, boundValues);
  }


  MultiArray::Pair Distribution::UnboundedSupport(const MultiArray::Array & paramValues) const
  {
    if (!CheckParamValues(paramValues))
      throw RuntimeError(String("Invalid parameters values in UnboundedSupport method for distribution ") + name_);

    return unboundedSupport(paramValues);
  }

}
