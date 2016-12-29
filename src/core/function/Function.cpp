#include "function/Function.hpp"
#include "iostream/std_ostream.hpp"

namespace Biips
{

  Bool Function::CheckParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckNParam(paramDims.size()))
      throw RuntimeError(String("Incorrect number of parameters for function ")
          + name_);

    return checkParamDims(paramDims);
  }

  DimArray Function::Dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    if (!CheckParamDims(paramDims)) {
      
      throw RuntimeError(String("Non-conforming parameters dimensions for function ")
          + name_);
    }
    return dim(paramDims).Drop();
  }

  void Function::Eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    if (!CheckParamValues(paramValues)) {
      throw RuntimeError(String("Invalid parameters values for function ")
          + name_ + ": " + print(paramValues));
    }

    eval(values, paramValues);
  }
}
