//                                               -*- C++ -*-
/*! \file version.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "version.hpp"

namespace Biips
{
  const String & BIIPS_VERSION()
  {
    static const String VERSION = "0.6.3";
    return VERSION;
  }
}
