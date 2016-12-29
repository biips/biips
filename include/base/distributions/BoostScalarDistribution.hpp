#ifndef BIIPS_BOOSTSCALARDISTRIBUTION_HPP_
#define BIIPS_BOOSTSCALARDISTRIBUTION_HPP_

#include "distributions/BoundedScalarDistribution.hpp"
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  template<typename MathDist, typename RandomDist>
  class BoostScalarDistribution: public BoundedScalarDistribution
  {
  public:
    typedef BoundedScalarDistribution BaseType;
    typedef MathDist MathDistType;
    typedef RandomDist RandomDistType;

  protected:
    virtual Bool
    checkDensityParamValues(Scalar x, const NumArray::Array & paramValues) const;

    virtual Scalar d(Scalar x,
                     const NumArray::Array & paramValues,
                     Bool give_log) const;
    virtual Scalar p(Scalar x,
                     const NumArray::Array & paramValues,
                     Bool lower,
                     Bool give_log) const;
    virtual Scalar q(Scalar p,
                     const NumArray::Array & paramValues,
                     Bool lower,
                     Bool log_p) const;
    virtual Scalar r(const NumArray::Array & paramValues, Rng & rng) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const = 0;
    virtual RandomDistType
    randomDist(const NumArray::Array & paramValues) const = 0;

    BoostScalarDistribution(const String & name,
                            Size nParam,
                            Support support,
                            Bool discrete = false) :
      BaseType(name, nParam, support, discrete)
    {
    }
  };

  template<typename MathDist, typename RandomDist>
  Bool BoostScalarDistribution<MathDist, RandomDist>::checkDensityParamValues(Scalar x,
                                                          const NumArray::Array & paramValues) const
  {
    return CheckParamValues(paramValues);
  }

  template<typename MathDist, typename RandomDist>
  Scalar BoostScalarDistribution<MathDist, RandomDist>::d(Scalar x,
                                                          const NumArray::Array & paramValues,
                                                          Bool give_log) const
  {
    MathDistType dist = mathDist(paramValues);

    if (give_log)
      return std::log(boost::math::pdf(dist, x));

    return boost::math::pdf(dist, x);
  }

  template<typename MathDist, typename RandomDist>
  Scalar BoostScalarDistribution<MathDist, RandomDist>::p(Scalar x,
                                                          const NumArray::Array & paramValues,
                                                          Bool lower,
                                                          Bool give_log) const
  {
    MathDistType dist = mathDist(paramValues);

    Scalar p;
    if (lower)
      p = boost::math::cdf(dist, x);
    else
      p = boost::math::cdf(boost::math::complement(dist, x));

    if (give_log)
      return std::log(p);
    else
      return p;
  }

  template<typename MathDist, typename RandomDist>
  Scalar BoostScalarDistribution<MathDist, RandomDist>::q(Scalar p,
                                                          const NumArray::Array & paramValues,
                                                          Bool lower,
                                                          Bool log_p) const
  {
    MathDistType dist = mathDist(paramValues);

    if (log_p)
      p = std::exp(p);

    if (lower)
      return boost::math::quantile(dist, p);
    else
      return boost::math::quantile(boost::math::complement(dist, p));
  }

  template<typename MathDist, typename RandomDist>
  Scalar BoostScalarDistribution<MathDist, RandomDist>::r(const NumArray::Array & paramValues,
                                                          Rng & rng) const
  {
    typedef boost::variate_generator<Rng::GenType&, RandomDistType> GenType;
    GenType gen(rng.GetGen(), randomDist(paramValues));

    return gen();
  }
}

#endif /* BIIPS_BOOSTSCALARDISTRIBUTION_HPP_ */
