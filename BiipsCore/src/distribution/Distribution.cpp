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
      throw LogicError(String("Incorrect number of parameters for distribution ")
          + name_);

    return checkParamDims(paramDims);
  }
  ;

  Bool Distribution::CheckParamValues(const NumArray::Array & paramValues) const
  {
    //    Types<DimArray::Ptr>::Array param_dims(paramValues.size());
    //    for (Size i = 0; i<paramValues.size(); ++i)
    //      param_dims[i] = paramValues[i].DimPtr();
    //
    //    if (!CheckParamDims(param_dims))
    //      throw LogicError(String("Non-conforming parameters dimensions for distribution ") + name_);

    return checkParamValues(paramValues);
  }

  Bool Distribution::CheckDensityParamValues(const NumArray & x,
                                             const NumArray::Array & paramValues) const
  {
    return CheckParamValues(paramValues);
  }

  DimArray Distribution::Dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckParamDims(paramDims))
      throw RuntimeError(String("Non-conforming parameters dimensions for distribution ")
          + name_);

    return dim(paramDims).Drop();
  }

  void Distribution::Sample(ValArray & values,
                            const NumArray::Array & paramValues,
                            const NumArray::Pair & boundValues,
                            Rng & rng) const
  {
    if (!CheckParamValues(paramValues))
      throw RuntimeError(String("Invalid parameters values in Sample method for distribution ")
          + name_);

    sample(values, paramValues, boundValues, rng);
  }

  Scalar Distribution::LogDensity(const NumArray & x,
                                  const NumArray::Array & paramValues,
                                  const NumArray::Pair & boundValues) const
  {
    if (!CheckDensityParamValues(x, paramValues))
      throw RuntimeError(String("Invalid parameters values in LogDensity method for distribution ")
          + name_);

    return logDensity(x, paramValues, boundValues);
  }

  void Distribution::UnboundedSupport(ValArray & lower,
                                      ValArray & upper,
                                      const NumArray::Array & paramValues) const
  {
    if (!CheckParamValues(paramValues))
      throw RuntimeError(String("Invalid parameters values in UnboundedSupport method for distribution ")
          + name_);

    unboundedSupport(lower, upper, paramValues);
  }

}
