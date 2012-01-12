//                                               -*- C++ -*-
/*! \file DPois.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DPOIS_HPP_
#define BIIPS_DPOIS_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/poisson_distribution.hpp>
#include <boost/math/distributions/poisson.hpp>

namespace Biips
{

  typedef boost::math::poisson_distribution<Scalar> PoisMathDistType;
  typedef boost::poisson_distribution<Int, Scalar> PoisRandomDistType;

  class DPois: public BoostScalarDistribution<PoisMathDistType,
      PoisRandomDistType>
  {
  public:
    typedef DPois SelfType;
    typedef BoostScalarDistribution<PoisMathDistType, PoisRandomDistType>
        BaseType;

  protected:
    DPois() :
      BaseType("dpois", 1, DIST_POSITIVE, true)
    {
    }
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;
    virtual Bool
    checkDensityParamValues(Scalar x, const NumArray::Array & paramValues) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    Scalar d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const;

    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_DPOIS_HPP_ */
