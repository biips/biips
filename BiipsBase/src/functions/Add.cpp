//                                               -*- C++ -*-
/*! \file Add.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */
#include "functions/Add.hpp"

namespace Biips
{

  Bool Add::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }


  Bool Add::IsLinear(const Flags & linearMask, const Flags & knownMask) const
  {
    Size n_lin = 0;
    for (Size i=0; i<linearMask.size(); ++i)
    {
      n_lin += linearMask[i];
      if (!linearMask[i] && !knownMask[i])
        return false;
    }

    return n_lin > 0;
  }

}
