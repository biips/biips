#ifndef BIIPS_DGAMMA_HPP_
#define BIIPS_DGAMMA_HPP_

#include <boost/random/gamma_distribution.hpp>
#include <boost/math/distributions/gamma.hpp>
#include "distributions/BoostScalarDistribution.hpp"

namespace Biips
{

  typedef boost::math::gamma_distribution<Scalar> GammaMathDistType;
  typedef boost::random::gamma_distribution<Scalar> GammaRandomDistType;

  class DGamma: public BoostScalarDistribution<GammaMathDistType,
      GammaRandomDistType>
  {
  public:
    typedef DGamma SelfType;
    typedef BoostScalarDistribution<GammaMathDistType, GammaRandomDistType>
        BaseType;

  protected:
    DGamma() :
      BaseType("dgamma", 2, DIST_POSITIVE, false)
    {
    }

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;
    virtual Scalar d(Scalar x,
                     const NumArray::Array & paramValues,
                     Bool give_log) const;
    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_DGAMMA_HPP_ */
