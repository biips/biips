#include "distribution/Distribution.hpp"
#include "iostream/std_ostream.hpp"

namespace Biips
{

  Bool Distribution::CheckParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckNParam(paramDims.size()))
      throw LogicError(String("Incorrect number of parameters for distribution ")
          + name_);

    return checkParamDims(paramDims);
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
          + name_ + ": " + print(paramValues));

    sample(values, paramValues, boundValues, rng);
  }

  Scalar Distribution::LogDensity(const NumArray & x,
                                  const NumArray::Array & paramValues,
                                  const NumArray::Pair & boundValues) const
  {
    if (!CheckDensityParamValues(x, paramValues))
      throw RuntimeError(String("Invalid parameters values in LogDensity method for distribution ")
          + name_ + ": " + print(paramValues));

    return logDensity(x, paramValues, boundValues);
  }

  void Distribution::FixedUnboundedSupport(ValArray & lower,
                                      ValArray & upper,
                                      const NumArray::Array & fixedParamValues) const
  {
    // FIXME
//    if (!CheckParamValues(fixedParamValues))
//      throw RuntimeError(String("Invalid parameters values in fixedUnboundedSupport method for distribution ")
//          + name_ + ": " + print(paramValues));

    fixedUnboundedSupport(lower, upper, fixedParamValues);
  }

}
