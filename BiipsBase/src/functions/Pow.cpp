//                                               -*- C++ -*-
/*! \file Pow.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Pow.hpp"

namespace Biips
{

  Bool Pow::checkParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & base = paramValues[0];
    const NumArray & exponent = paramValues[1];

    for (Size i = 0, j = 0; i < base.Values().size(); ++i)
    {
      if ((base.Values()[i] < 0.0) && !checkInteger(exponent.Values()[j]))
        return false;

      if ((base.Values()[i] == 0.0) && (exponent.Values()[j] < 0.0)) // FIXME use epsilon in equal comparison
        return false;

      if (!exponent.IsScalar())
        ++j;
    }
    return true;
  }

  Bool PowInfix::checkParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & base = paramValues[0];
    const NumArray & exponent = paramValues[1];

    for (Size i = 0, j = 0; i < base.Values().size(); ++i)
    {
      if ((base.Values()[i] < 0.0) && !checkInteger(exponent.Values()[j]))
        return false;

      if ((base.Values()[i] == 0.0) && (exponent.Values()[j] < 0.0)) // FIXME use epsilon in equal comparison
        return false;

      if (!exponent.IsScalar())
        ++j;
    }
    return true;
  }
}
