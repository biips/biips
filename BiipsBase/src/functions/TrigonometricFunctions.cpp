//                                               -*- C++ -*-
/*! \file TrigonometricFunctions.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/TrigonometricFunctions.hpp"

namespace Biips
{

  Bool ArcCos::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i=0; i<x.size(); ++i)
    {
      if (x[i] < -1.0 || x[i] > 1.0)
        return false;
    }
    return true;
  }


  Bool ArcSin::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i=0; i<x.size(); ++i)
    {
      if (x[i] < -1.0 || x[i] > 1.0)
        return false;
    }
    return true;
  }


  Bool ArcCosh::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i=0; i<x.size(); ++i)
    {
      if (x[i] < 1.0)
        return false;
    }
    return true;
  }


  Bool ArcTanh::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();
    for (Size i=0; i<x.size(); ++i)
    {
      if (x[i] <= -1.0 || x[i] >= 1.0)
        return false;
    }
    return true;
  }
}
