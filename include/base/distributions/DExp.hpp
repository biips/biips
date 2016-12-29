#ifndef BIIPS_DEXP_HPP_
#define BIIPS_DEXP_HPP_

#include <boost/random/exponential_distribution.hpp>
#include <boost/math/distributions/exponential.hpp>
#include "distributions/BoostScalarDistribution.hpp"

namespace Biips
{

  typedef boost::math::exponential_distribution<Scalar> ExpMathDistType;
  typedef boost::exponential_distribution<Scalar> ExpRandomDistType;

  class DExp: public BoostScalarDistribution<ExpMathDistType, ExpRandomDistType>
  {
  public:
    typedef DExp SelfType;
    typedef BoostScalarDistribution<ExpMathDistType, ExpRandomDistType>
        BaseType;

  protected:
    DExp() :
      BaseType("dexp", 1, DIST_POSITIVE, false)
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

#endif /* BIIPS_DEXP_HPP_ */
