//                                               -*- C++ -*-
/*! \file MultiArray.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "common/MultiArray.hpp"
#include "common/Vector.hpp"
#include "common/Matrix.hpp"

namespace Biips
{
  Bool allMissing(const MultiArray & marray)
  {
    Size N = marray.Length();
    const ValArray & v = marray.Values();
    for (Size i = 0; i < N; ++i)
    {
      if (v[i] != BIIPS_REALNA)
        return false;
    }
    return true;
  }

  Bool anyMissing(const MultiArray & marray)
  {
    Size N = marray.Length();
    const ValArray & v = marray.Values();
    for (Size i = 0; i < N; ++i)
    {
      if (v[i] == BIIPS_REALNA)
        return true;
    }
    return false;
  }

  Bool allMissing(const NumArray & marray)
  {
    Size N = marray.Length();
    const ValArray & v = marray.Values();
    for (Size i = 0; i < N; ++i)
    {
      if (v[i] != BIIPS_REALNA)
        return false;
    }
    return true;
  }

  Bool anyMissing(const NumArray & marray)
  {
    Size N = marray.Length();
    const ValArray & v = marray.Values();
    for (Size i = 0; i < N; ++i)
    {
      if (v[i] == BIIPS_REALNA)
        return true;
    }
    return false;
  }

}
