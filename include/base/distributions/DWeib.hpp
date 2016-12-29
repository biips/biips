#ifndef BIIPS_DWEIB_HPP_
#define BIIPS_DWEIB_HPP_

#include <boost/random/weibull_distribution.hpp>
#include <boost/math/distributions/weibull.hpp>
#include "distributions/BoostScalarDistribution.hpp"

namespace Biips
{

  typedef boost::math::weibull_distribution<Scalar> WeibMathDistType;
  typedef boost::random::weibull_distribution<Scalar> WeibRandomDistType;

  class DWeib: public BoostScalarDistribution<WeibMathDistType,
      WeibRandomDistType>
  {
  public:
    typedef DWeib SelfType;
    typedef BoostScalarDistribution<WeibMathDistType, WeibRandomDistType>
        BaseType;

  protected:
    DWeib() :
      BaseType("dweib", 2, DIST_POSITIVE, false)
    {
    }

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "dweibull";
    }
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

#endif /* DWEIB_HPP_ */
