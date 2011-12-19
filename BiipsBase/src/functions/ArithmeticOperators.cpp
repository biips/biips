//                                               -*- C++ -*-
/*! \file ArithmeticOperators.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */
#include "functions/ArithmeticOperators.hpp"

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
      if (!(linearMask[i] || knownMask[i]))
        return false;
    }

    return n_lin > 0;
  }


  Bool Subtract::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }


  Bool Subtract::IsLinear(const Flags & linearMask, const Flags & knownMask) const
  {
    for (Size i=0; i<linearMask.size(); ++i)
    {
      if (!(linearMask[i] || knownMask[i]))
        return false;
    }

    return linearMask[0] || linearMask[1];
  }


  Bool Multiply::IsDiscreteValued(const Flags & mask) const
  {
    return allTrue(mask);
  }


  // FIXME: valid with element wise operation ?
  Bool Multiply::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    Size n_factor = 0;
    for (Size i=0; i<scaleMask.size(); ++i)
    {
      n_factor += scaleMask[i];
      if (!(scaleMask[i] || knownMask[i]))
        return false;
    }

    return n_factor == 1;
  }


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


  // FIXME: valid with element wise operation ?
  Bool Divide::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    return scaleMask[0] && knownMask[1];
  }


  Bool Neg::IsDiscreteValued(const Flags & mask) const
  {
    return mask[0];
  }


  Bool Neg::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    return scaleMask[0];
  }

}
