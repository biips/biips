#ifndef BIIPS_DBETA_HPP_
#define BIIPS_DBETA_HPP_

#include "boost/random/beta_distribution.hpp"
#include <boost/math/distributions/beta.hpp>

#include "distributions/BoostScalarDistribution.hpp"

namespace Biips
{
  typedef boost::math::beta_distribution<Scalar> BetaMathDistType;
  typedef boost::beta_distribution<Scalar> BetaRandomDistType;

  class DBeta: public BoostScalarDistribution<BetaMathDistType,
      BetaRandomDistType>
  {
  public:
    typedef DBeta SelfType;
    typedef BoostScalarDistribution<BetaMathDistType, BetaRandomDistType>
        BaseType;

  protected:
    DBeta() :
      BaseType("dbeta", 2, DIST_PROPORTION, false)
    {
    }
    virtual Bool
    checkDensityParamValues(Scalar x, const NumArray::Array & paramValues) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    Scalar d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const;
    Scalar r(const NumArray::Array & paramValues, Rng & rng) const;

    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;

    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
    ;
  };

}

#endif /* BIIPS_DBETA_HPP_ */
