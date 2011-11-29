//                                               -*- C++ -*-
/*! \file UsualFunctions.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/UsualFunctions.hpp"

namespace Biips
{

  Bool Sqrt::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();

    for (Size i=0; i<x.size(); ++i)
    {
      if (x[i] < 0.0)
        return false;
    }

    return true;
  }

  Bool Log::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();

    for (Size i=0; i<x.size(); ++i)
    {
      if (x[i] <= 0.0)
        return false;
    }

    return true;
  }
}
