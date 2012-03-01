//                                               -*- C++ -*-
/*! \file Sum.cpp
 * \brief
 *
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Sum.hpp"

namespace Biips
{

  Bool Sum::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return true;
  }

  void Sum::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    const NumArray & val = paramValues[0];

    values[0] = val.Values().Sum();
  }

  Bool Sum::IsDiscreteValued(const Flags & mask) const
  {
    return mask[0];
  }
}
