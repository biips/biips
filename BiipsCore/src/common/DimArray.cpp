//                                               -*- C++ -*-
/*! \file DimArray.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "common/DimArray.hpp"

namespace Biips
{

  DimArray DimArray::Drop() const
  {
    DimArray dimDropped;
    for (Size i = 0; i < size(); ++i)
      if ( (*this)[i] > 1 )
        dimDropped.push_back((*this)[i]);
    return  dimDropped.empty() && !empty() ? *P_SCALAR_DIM : dimDropped;
  }
}
