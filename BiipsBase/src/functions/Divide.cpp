//                                               -*- C++ -*-
/*! \file Divide.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Divide.hpp"

namespace Biips
{

  Bool Divide::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const MultiArray & denominator = paramValues[1];
    for (Size i=0; i<denominator.Values().size(); ++i)
    {
      if (denominator.Values()[i] == 0.0) // FIXME use epsilon error comparison
        return false;
    }
    return true;
  }


  // TODO: valid with element wise operation ?
  Bool Divide::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    return scaleMask[0] && knownMask[1];
  }
}
