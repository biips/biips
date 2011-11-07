//                                               -*- C++ -*-
/*! \file Neg.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Neg.hpp"

namespace Biips
{

  Bool Neg::IsDiscreteValued(const Flags & mask) const
  {
    return mask[0];
  }


  Bool Neg::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    return scaleMask[0];
  }

}
