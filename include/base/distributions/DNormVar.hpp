#ifndef BIIPS_DNORMVAR_HPP_
#define BIIPS_DNORMVAR_HPP_

#include <boost/random/normal_distribution.hpp>
#include <boost/math/distributions/normal.hpp>
#include "distributions/BoostScalarDistribution.hpp"

namespace Biips
{
  typedef boost::math::normal_distribution<Scalar> NormalMathDistType;
  typedef boost::normal_distribution<Scalar> NormalRandomDistType;

  class DNormVar: public BoostScalarDistribution<NormalMathDistType,
      NormalRandomDistType>
  {
  public:
    typedef DNormVar SelfType;
    typedef BoostScalarDistribution<NormalMathDistType, NormalRandomDistType>
        BaseType;

  protected:
    DNormVar() :
      BaseType("dnormvar", 2, DIST_UNBOUNDED, false)
    {
    }
    virtual void sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const;
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

#endif /* BIIPS_DNORMVAR_HPP_ */
