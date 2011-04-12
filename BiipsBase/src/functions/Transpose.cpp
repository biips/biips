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


  MultiArray Transpose::Eval(const MultiArray::Array & paramValues) const
  {
    // TODO check paramValues
    const MultiArray & val = paramValues[0];

    MultiArray ans(ublas::trans(Matrix(val)));
    return ans;
  }

  DimArray Transpose::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    DimArray dim(2);
    dim[0] = (*paramDims[0])[1];
    dim[1] = (*paramDims[0])[0];
    return dim;
  }
}
