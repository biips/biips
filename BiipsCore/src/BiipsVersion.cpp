//                                               -*- C++ -*-
/*! \file BiipsVersion.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "BiipsConfig.hpp"
#include "BiipsVersion.hpp"
#include "common/Utility.hpp"

namespace Biips
{
  const String & BIIPS_VERSION()
  {
    static const String VERSION = Biips_VERSION;
    return VERSION;
  }
}
