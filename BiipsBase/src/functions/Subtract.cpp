//                                               -*- C++ -*-
/*! \file Subtract.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Subtract.hpp"

namespace Biips
{

  Bool Subtract::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }


  Bool Subtract::IsLinear(const Flags & linearMask, const Flags & knownMask) const
  {
    Size n_lin = 0;

    for (Size i=0; i<linearMask.size(); ++i)
    {
      n_lin += linearMask[i];
      if (!linearMask[i] && !knownMask[i])
        return false;
    }

    return n_lin>0;
  }

}
