//                                               -*- C++ -*-
/*! \file BoostScalarDistribution.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_BOOSTSCALARDISTRIBUTION_HPP_
#define BIIPS_BOOSTSCALARDISTRIBUTION_HPP_

#include "distributions/BoundedScalarDistribution.hpp"
#include <boost/random/variate_generator.hpp>

namespace boost
{
  namespace math
  {
  }
}

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
                     Bool give_log) const;
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
  Scalar BoostScalarDistribution<MathDist, RandomDist>::d(Scalar x,
                                                          const NumArray::Array & paramValues,
                                                          Bool give_log) const
  {
    MathDistType dist = mathDist(paramValues);

    using namespace boost::math;
    if (give_log)
      return std::log(pdf(dist, x));

    return pdf(dist, x);
  }

  template<typename MathDist, typename RandomDist>
  Scalar BoostScalarDistribution<MathDist, RandomDist>::p(Scalar x,
                                                          const NumArray::Array & paramValues,
                                                          Bool lower,
                                                          Bool give_log) const
  {
    MathDistType dist = mathDist(paramValues);

    using namespace boost::math;
    Scalar p;
    if (lower)
      p = cdf(dist, x);
    else
      p = cdf(complement(dist, x));

    if (give_log)
      return std::log(p);
    else
      return p;
  }

  template<typename MathDist, typename RandomDist>
  Scalar BoostScalarDistribution<MathDist, RandomDist>::q(Scalar p,
                                                          const NumArray::Array & paramValues,
                                                          Bool lower,
                                                          Bool give_log) const
  {
    MathDistType dist = mathDist(paramValues);

    using namespace boost::math;
    Scalar q;
    if (lower)
      q = quantile(dist, p);
    else
      q = quantile(complement(dist, p));

    if (give_log)
      return std::log(q);
    else
      return q;
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
