/*! \file Transpose.cpp
 * COPY: Adapted from JAGS Transpose.cc
 */

#include "functions/Transpose.hpp"

namespace Biips
{
  Bool Transpose::checkParamDims(
      const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return paramDims[0]->IsVector() || paramDims[0]->IsMatrix();
  }

  void Transpose::eval(ValArray & values,
                       const NumArray::Array & paramValues) const
  {
    Size nrow = paramValues[0].Dim()[0];
    Size ncol = paramValues[0].IsMatrix() ? paramValues[0].Dim()[1] : 1;
    Size length = nrow * ncol;
    for (Size i = 0; i < length; ++i)
    {
      values[i] = paramValues[0].Values()[(i / ncol) + (i % ncol) * nrow];
    }

  }

  DimArray Transpose::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    DimArray dim(2);
    dim[0] = paramDims[0]->IsMatrix() ? (*paramDims[0])[1] : 1;
    dim[1] = (*paramDims[0])[0];
    return dim;
  }
}
