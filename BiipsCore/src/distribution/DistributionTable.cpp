//                                               -*- C++ -*-
/*! \file DistributionTable.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distribution/DistributionTable.hpp"

namespace Biips
{
  const Distribution::Ptr DistributionTable::nullDist_;

  Bool DistributionTable::Insert(const Distribution::Ptr & pDist)
  {
    // TODO check pDist
    if (Contains(pDist->Name()))
      return false;
    distMap_.insert(std::make_pair(pDist->Name(), pDist));
    return true;
  }

  Bool DistributionTable::Contains(const String & distName) const
  {
    return distMap_.find(distName) != distMap_.end();
  }

  const Distribution::Ptr & DistributionTable::GetDistributionPtr(const String & distName) const
  {
    if (Contains(distName))
      return distMap_.at(distName);
    else
      return nullDist_;
  }
}
