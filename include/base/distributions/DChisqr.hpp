#ifndef BIIPS_DCHISQR_HPP_
#define BIIPS_DCHISQR_HPP_

#include <boost/random/chi_squared_distribution.hpp>
#include <boost/math/distributions/chi_squared.hpp>
#include "distributions/BoostScalarDistribution.hpp"

namespace Biips
{

  typedef boost::math::chi_squared_distribution<Scalar> ChisqrMathDistType;
  typedef boost::random::chi_squared_distribution<Scalar> ChisqrRandomDistType;

  class DChisqr: public BoostScalarDistribution<ChisqrMathDistType,
      ChisqrRandomDistType>
  {
  public:
    typedef DChisqr SelfType;
    typedef BoostScalarDistribution<ChisqrMathDistType, ChisqrRandomDistType>
        BaseType;

  protected:
    DChisqr() :
      BaseType("dchisqr", 1, DIST_POSITIVE, false)
    {
    }

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "dchisq";
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

#endif /* BIIPS_DCHISQR_HPP_ */
