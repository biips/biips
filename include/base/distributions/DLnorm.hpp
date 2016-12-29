#ifndef BIIPS_DLNORM_HPP_
#define BIIPS_DLNORM_HPP_

#include <boost/random/lognormal_distribution.hpp>
#include <boost/math/distributions/lognormal.hpp>
#include "distributions/BoostScalarDistribution.hpp"

namespace Biips
{

  typedef boost::math::lognormal_distribution<Scalar> LognormMathDistType;
  typedef boost::random::lognormal_distribution<Scalar> LognormRandomDistType;

  class DLnorm: public BoostScalarDistribution<LognormMathDistType,
      LognormRandomDistType>
  {
  public:
    typedef DLnorm SelfType;
    typedef BoostScalarDistribution<LognormMathDistType, LognormRandomDistType>
        BaseType;

  protected:
    DLnorm() :
      BaseType("dlnorm", 2, DIST_POSITIVE, false)
    {
    }

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType
    randomDist(const NumArray::Array & paramValues) const;

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

#endif /* BIIPS_DLNORM_HPP_ */
