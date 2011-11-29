//                                               -*- C++ -*-
/*! \file DBeta.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DBETA_HPP_
#define BIIPS_DBETA_HPP_

#include "distributions/BoostScalarDistribution.hpp"

#include <boost/random/beta_distribution.hpp>
#include <boost/math/distributions/beta.hpp>

namespace Biips
{
  typedef boost::math::beta_distribution<Scalar> BetaMathDistType;
  typedef boost::beta_distribution<Scalar> BetaRandomDistType;

  class DBeta : public BoostScalarDistribution<BetaMathDistType, BetaRandomDistType>
  {
  public:
    typedef DBeta SelfType;
    typedef BoostScalarDistribution<BetaMathDistType, BetaRandomDistType> BaseType;

  protected:
    DBeta() : BaseType("dbeta", 2, DIST_PROPORTION, false) {}
    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const;
    virtual Bool checkDensityParamValues(Scalar x, const MultiArray::Array & paramValues) const;

    virtual MathDistType mathDist(const MultiArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const MultiArray::Array & paramValues) const;

  public:
    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_DBETA_HPP_ */
