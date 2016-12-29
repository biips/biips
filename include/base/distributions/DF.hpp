#ifndef BIIPS_DF_HPP_
#define BIIPS_DF_HPP_

#include <boost/random/fisher_f_distribution.hpp>
#include <boost/math/distributions/fisher_f.hpp>
#include "distributions/BoostScalarDistribution.hpp"

namespace Biips
{

  typedef boost::math::fisher_f_distribution<Scalar> FMathDistType;
  typedef boost::random::fisher_f_distribution<Scalar> FRandomDistType;

  class DF: public BoostScalarDistribution<FMathDistType, FRandomDistType>
  {
  public:
    typedef DF SelfType;
    typedef BoostScalarDistribution<FMathDistType, FRandomDistType> BaseType;

  protected:
    DF() :
      BaseType("df", 2, DIST_POSITIVE, false)
    {
    }
    virtual Bool
    checkDensityParamValues(Scalar x, const NumArray::Array & paramValues) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;
    Scalar d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const;

    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_DF_HPP_ */
