//                                               -*- C++ -*-
/*! \file Max.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Max.hpp"

namespace Biips
{

  void Max::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    Scalar ans = *std::max_element(paramValues[0].Values().begin(),
                                  paramValues[0].Values().end());
    for (Size i = 1; i < paramValues.size(); ++i)
    {
      Scalar max_i = *std::max_element(paramValues[i].Values().begin(),
                                      paramValues[i].Values().end());
      ans = std::max(ans, max_i);
    }
    values[0] = ans;
  }

  Bool Max::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }
}

