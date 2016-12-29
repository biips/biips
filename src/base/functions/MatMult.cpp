/*! \file MatMult.cpp
 * COPY: Adapted from JAGS MatMult.cc
 */

#include "functions/MatMult.hpp"

namespace Biips
{

  Bool MatMult::checkParamDims(
      const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];
    if ((left.size() > 2) || (right.size() > 2))
      return false;
    if (left.IsVector())
      return (left[0] == right[0]);
    else
      return (left[1] == right[0]);
  }

  void MatMult::eval(ValArray & values,
                     const NumArray::Array & paramValues) const
  {
    const NumArray & left = paramValues[0];
    const NumArray & right = paramValues[1];


    // FIXME: this is only valid for column major order
    Size d1, d2, d3;

    if (left.IsVector())
    {
      d1 = 1;
      d2 = left.Dim()[0];
    }
    else
    {
      d1 = left.Dim()[0];
      d2 = left.Dim()[1];
    }
    if (right.IsVector())
      d3 = 1;
    else
      d3 = right.Dim()[1];

    for (Size i = 0; i < d1; ++i)
    {
      for (Size j = 0; j < d3; ++j)
      {
        values[i + d1 * j] = 0;
        for (Size k = 0; k < d2; ++k)
        {
          values[i + d1 * j] += left.Values()[i + d1 * k]
                                * right.Values()[k + d2 * j];
        }
      }
    }

  }

  DimArray MatMult::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];

    DimArray dim(2, 1);
    if (left.IsMatrix())
      dim.push_back(left[0]);
    if (right.IsMatrix())
      dim.push_back(right[1]);
    return dim.Drop();
  }

  Bool MatMult::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    return (knownMask[0] && scaleMask[1]) || (scaleMask[0] && knownMask[1]);
  }
}
