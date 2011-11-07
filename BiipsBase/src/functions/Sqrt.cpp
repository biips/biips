//                                               -*- C++ -*-
/*! \file Sqrt.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Sqrt.hpp"

namespace Biips
{

  Bool Sqrt::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const MultiArray & x = paramValues[0];

    for (Size i=0; i<x.Values().size(); ++i)
    {
      if (x.Values()[i] < 0.0)
        return false;
    }

    return true;
  }
}
