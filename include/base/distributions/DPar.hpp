#ifndef BIIPS_DPAR_HPP_
#define BIIPS_DPAR_HPP_

#include "boost/random/pareto_distribution.hpp"
#include <boost/math/distributions/pareto.hpp>
#include "distributions/BoostScalarDistribution.hpp"

namespace Biips
{

  typedef boost::math::pareto_distribution<Scalar> ParMathDistType;
  typedef boost::pareto_distribution<Scalar> ParRandomDistType;

  class DPar: public BoostScalarDistribution<ParMathDistType, ParRandomDistType>
  {
  public:
    typedef DPar SelfType;
    typedef BoostScalarDistribution<ParMathDistType, ParRandomDistType>
        BaseType;

  protected:
    DPar() :
      BaseType("dpar", 2, DIST_SPECIAL, false)
    {
    }
    virtual Scalar fixedUnboundedLower(const NumArray::Array & paramValues) const;
    virtual Scalar fixedUnboundedUpper(const NumArray::Array & paramValues) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "dbinom";
    }
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;
    virtual Bool IsSupportFixed(const Flags & fixmask) const;
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

#endif /* BIIPS_DPAR_HPP_ */
