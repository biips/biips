//                                               -*- C++ -*-
/*! \file DGamma.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DGAMMA_HPP_
#define BIIPS_DGAMMA_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/gamma_distribution.hpp>
#include <boost/math/distributions/gamma.hpp>

namespace Biips
{

  typedef boost::math::gamma_distribution<Scalar> GammaMathDistType;
  typedef boost::gamma_distribution<Scalar> GammaRandomDistType;

  class DGamma : public BoostScalarDistribution<GammaMathDistType, GammaRandomDistType>
  {
  public:
    typedef DGamma SelfType;
    typedef BoostScalarDistribution<GammaMathDistType, GammaRandomDistType> BaseType;

  protected:
    DGamma() : BaseType("dgamma", 2, DIST_POSITIVE, false) {}
    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const;

    virtual MathDistType mathDist(const MultiArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const MultiArray::Array & paramValues) const;

  public:
    virtual Scalar d(Scalar x, const MultiArray::Array & paramValues,
        Bool give_log) const;
    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; }
  };

}

#endif /* BIIPS_DGAMMA_HPP_ */
