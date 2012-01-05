//                                               -*- C++ -*-
/*! \file Transpose.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Transpose.hpp"

namespace Biips
{
  Bool Transpose::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return paramDims[0]->IsMatrix();
  }

  void Transpose::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    const NumArray & val = paramValues[0];

    Matrix t = ublas::trans(Matrix(val));

    values.swap(t.data());
  }

  DimArray Transpose::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    DimArray dim(2);
    dim[0] = (*paramDims[0])[1];
    dim[1] = (*paramDims[0])[0];
    return dim;
  }
}
