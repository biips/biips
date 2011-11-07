//                                               -*- C++ -*-
/*! \file Multiply.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Multiply.hpp"

namespace Biips
{

  Bool Multiply::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }


  // TODO: valid with element wise operation ?
  Bool Multiply::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    Size n_factor = 0;
    for (Size i=0; i<scaleMask.size(); ++i)
    {
      n_factor += scaleMask[i];
      if (!scaleMask[i] && !knownMask[i])
        return false;
    }

    return n_factor == 1;
  }

}
