/*! \file BoundedScalarDistribution.hpp
 * COPY: Adapted from JAGS RScalarDist
 */

#ifndef BIIPS_BOUNDEDSCALARDISTRIBUTION_HPP_
#define BIIPS_BOUNDEDSCALARDISTRIBUTION_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{

  /**
   * Enumerates three possible ranges of support for a scalar random
   * variable.
   *
   * DIST_UNBOUNDED for support on the whole real line
   *
   * DIST_POSITIVE for support on values > 0
   *
   * DIST_PROPORTION for support on values in [0,1]
   *
   * DIST_SPECIAL for other distributions, e.g. distributions where the
   * support depends on the parameter values.
   */
  enum Support
  {
    DIST_UNBOUNDED, DIST_POSITIVE, DIST_PROPORTION, DIST_SPECIAL
  };

  class BoundedScalarDistribution: public Distribution
  {
  public:
    typedef Distribution BaseType;
    typedef BoundedScalarDistribution SelfType;

  protected:
    const Support support_;
    const Bool discrete_;
    Scalar calPlower(Scalar lower, const NumArray::Array & paramValues) const;
    Scalar calPupper(Scalar upper, const NumArray::Array & paramValues) const;

    virtual Bool
    checkDensityParamValues(Scalar x, const NumArray::Array & paramValues) const;
    Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return *P_SCALAR_DIM;
    }
    virtual void sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const;
    virtual Scalar logDensity(const NumArray & x,
                              const NumArray::Array & paramValues,
                              const NumArray::Pair & boundValues) const;
    virtual Scalar fixedUnboundedLower(const NumArray::Array & paramValues) const;
    virtual Scalar fixedUnboundedUpper(const NumArray::Array & paramValues) const;
    virtual void
    fixedUnboundedSupport(ValArray & lower,
                     ValArray & upper,
                     const NumArray::Array & paramValues) const;

    BoundedScalarDistribution(const String & name,
                              Size nParam,
                              Support support,
                              Bool discrete = false) :
      BaseType(name, nParam), support_(support), discrete_(discrete)
    {
    }

  public:
    /**
     * Density function, ignoring bounds
     * @param x value at which to evaluate the density
     * @param paramValues Array of paramValues
     * @param give_log Indicates whether to return log density.
     */
    virtual Scalar d(Scalar x,
                     const NumArray::Array & paramValues,
                     Bool give_log) const = 0;
    /**
     * Distribution function, ignoring bounds
     * @param x quantile at which to evaluate the distribution function
     * @param paramValues Array of parameters
     * @param lower If true, return value is P[X <= x]. Otherwise
     * P[X > x]
     * @param give_log Indicates whether to return log probability
     */
    virtual Scalar p(Scalar x,
                     const NumArray::Array & paramValues,
                     Bool lower,
                     Bool give_log) const = 0;
    /**
     * Quantile function, ignoring bounds
     * @param p probability for which to evaluate quantile
     * @param parameters Array of parameters
     * @param give_log Indicates whether p is given as log(p).
     */
    virtual Scalar q(Scalar p,
                     const NumArray::Array & paramValues,
                     Bool lower,
                     Bool log_p) const = 0;
    /**
     * Random number generation, ignoring bounds
     * @param parameters Array of parameters
     */
    virtual Scalar r(const NumArray::Array & paramValues, Rng & rng) const = 0;

    virtual Bool
    CheckDensityParamValues(const NumArray & x,
                            const NumArray::Array & paramValues) const;
    virtual Bool CanBound() const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const
    {
      return discrete_;
    }
    virtual Bool IsSupportFixed(const Flags & fixmask) const;
    virtual Size DegreeOfFreedom(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return 1;
    }

    virtual ~BoundedScalarDistribution()
    {
    }
  };

}

#endif /* BIIPS_BOUNDEDSCALARDISTRIBUTION_HPP_ */
