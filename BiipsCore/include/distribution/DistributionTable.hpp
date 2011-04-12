//                                               -*- C++ -*-
/*! \file DistributionTable.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DISTRIBUTIONTABLE_HPP_
#define BIIPS_DISTRIBUTIONTABLE_HPP_

#include "distribution/Distribution.hpp"
#include <map>

namespace Biips
{

  // TODO Names of distributions are stored twice:
  // once in the name_ field of the Distribution object
  // onces in the first key of the distMap_ field
  // of the DistributionTable object.
  // This can be optimized.
  class DistributionTable
  {
  protected:
    std::map<String, Distribution::Ptr> distMap_;

    static const Distribution::Ptr nullDist_;

  public:
    DistributionTable() {}

    Bool Insert(const Distribution::Ptr & pDist);

    Bool Contains(const String & distName) const;

    const Distribution::Ptr & GetDistributionPtr(const String & distName) const;
    const Distribution::Ptr & operator [] (const String & distName) const { return GetDistributionPtr(distName); };
  };

}

#endif /* BIIPS_DISTRIBUTIONTABLE_HPP_ */
