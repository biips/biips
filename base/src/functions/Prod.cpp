//                                               -*- C++ -*-
/*! \file Prod.cpp
 * \brief
 *
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Prod.hpp"

namespace Biips
{

  Bool Prod::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return true;
  }

  void Prod::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    const NumArray & val = paramValues[0];

    values[0] = std::accumulate(val.Values().begin(),
                                val.Values().end(),
                                1.0,
                                std::multiplies<Scalar>());
  }

  Bool Prod::IsDiscreteValued(const Flags & mask) const
  {
    return mask[0];
  }
}
