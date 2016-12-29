/*! \file BoundedScalarDistribution.cpp
 * COPY: Adapted from JAGS RScalarDist
 */

#include "distributions/BoundedScalarDistribution.hpp"
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  Bool BoundedScalarDistribution::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    for (Size i = 0; i < paramDims.size(); ++i)
    {
      if (!paramDims[i]->IsScalar())
        return false;
    }
    return true;
  }

  Scalar BoundedScalarDistribution::calPlower(Scalar lower,
                                              const NumArray::Array & paramValues) const
  {
    //P(X < lower)
    if (discrete_)
      return p(lower - 1, paramValues, true, false); // FIXME : check this
    else
      return p(lower, paramValues, true, false);
  }

  Scalar BoundedScalarDistribution::calPupper(Scalar upper,
                                              const NumArray::Array & paramValues) const
  {
    //P(X <= upper)
    return p(upper, paramValues, true, false);
  }

  Scalar BoundedScalarDistribution::logDensity(const NumArray & x,
                                               const NumArray::Array & paramValues,
                                               const NumArray::Pair & boundValues) const
  {
    const NumArray & lower = boundValues.first;
    const NumArray & upper = boundValues.second;

    if (!lower.IsNULL() && x.ScalarView() < lower.ScalarView())
      return BIIPS_NEGINF;
    if (!upper.IsNULL() && x.ScalarView() > upper.ScalarView())
      return BIIPS_NEGINF;
    if (!upper.IsNULL() && !lower.IsNULL() && upper.ScalarView()
        < lower.ScalarView())
      return BIIPS_NEGINF;

    Scalar log_dens = d(x.ScalarView(), paramValues, true);

    if (!lower.IsNULL() || !upper.IsNULL())
    {
      //Normalize truncated distributions

      //Make adjustment for discrete-valued distributions
      Scalar ll = 0.0;
      if (!lower.IsNULL())
        ll = discrete_ ? lower.ScalarView() - 1 : lower.ScalarView(); // FIXME : check this

      /* In theory, we just have to subtract log[P(lower <= X <=
       upper)] from the log likelihood. But we need to work around
       numerical problems. */

      Bool have_lower = !lower.IsNULL() && p(ll, paramValues, true, false)
          > 0.0;
      Bool have_upper = !upper.IsNULL() && p(upper.ScalarView(),
                                             paramValues,
                                             false,
                                             false) > 0.0;

      if (have_lower && have_upper)
      {
        if (p(ll, paramValues, false, false) < 0.5)
        {
          //Use upper tail
          log_dens -= std::log(p(ll, paramValues, false, false)
              - p(upper.ScalarView(), paramValues, false, false));
        }
        else
        {
          //Use lower tail
          log_dens -= std::log(p(upper.ScalarView(), paramValues, true, false)
              - p(ll, paramValues, true, false));
        }
      }
      else if (have_lower)
        log_dens -= p(ll, paramValues, false, true);

      else if (have_upper)
        log_dens -= p(upper.ScalarView(), paramValues, true, true);
    }

    return log_dens;
  }

  void BoundedScalarDistribution::sample(ValArray & values,
                                         const NumArray::Array & paramValues,
                                         const NumArray::Pair & boundValues,
                                         Rng & rng) const
  {
    const NumArray & lower = boundValues.first;
    const NumArray & upper = boundValues.second;

    if (!lower.IsNULL() || !upper.IsNULL())
    {
      Scalar plower = 0.0, pupper = 1.0;
      if (!lower.IsNULL())
        plower = calPlower(lower.ScalarView(), paramValues);

      if (!upper.IsNULL())
        pupper = calPupper(upper.ScalarView(), paramValues);

      typedef boost::uniform_real<Scalar> UniformDist;
      UniformDist unif_dist(plower, pupper);
      typedef boost::variate_generator<Rng::GenType&, UniformDist> UniformGen;
      UniformGen unif_gen(rng.GetGen(), unif_dist);

      Scalar u = unif_gen();
      values[0] = q(u, paramValues, true, false);
    }
    else
      values[0] = r(paramValues, rng);
  }

  Scalar BoundedScalarDistribution::fixedUnboundedLower(const NumArray::Array & paramValues) const
  {
    switch (support_)
    {
      case DIST_UNBOUNDED:
        return BIIPS_NEGINF;
        break;
      case DIST_POSITIVE:
      case DIST_PROPORTION:
        return 0.0;
        break;
      case DIST_SPECIAL:
        //You must overload this function
        throw LogicError("Cannot call BoundedScalarDistribution::unboundedLower for special distribution");
    }
    return 0.0; //Wall
  }

  Scalar BoundedScalarDistribution::fixedUnboundedUpper(const NumArray::Array & paramValues) const
  {
    switch (support_)
    {
      case DIST_UNBOUNDED:
      case DIST_POSITIVE:
        return BIIPS_POSINF;
        break;
      case DIST_PROPORTION:
        return 1.0;
        break;
      case DIST_SPECIAL:
        //You must overload this function
        throw LogicError("Cannot call BoundedScalarDistribution::unboundedUpper for special distribution");
    }
    return 0.0; //Wall
  }

  void BoundedScalarDistribution::fixedUnboundedSupport(ValArray & lower,
                                                   ValArray & upper,
                                                   const NumArray::Array & paramValues) const
  {
    lower[0] = fixedUnboundedLower(paramValues);
    upper[0] = fixedUnboundedUpper(paramValues);
  }

  Bool BoundedScalarDistribution::CheckDensityParamValues(const NumArray & x,
                                                          const NumArray::Array & paramValues) const
  {
    return checkDensityParamValues(x.ScalarView(), paramValues);
  }

  Bool BoundedScalarDistribution::checkDensityParamValues(Scalar x,
                                                          const NumArray::Array & paramValues) const
  {
    return CheckParamValues(paramValues);
  }

  Bool BoundedScalarDistribution::IsSupportFixed(const Flags & fixmask) const
  {
    if (support_ == DIST_SPECIAL)
    {
      //You must overload this function
      throw LogicError("Cannot call BoundedScalarDistribution::IsSupportFixed for special distribution");
    }
    else
      return true;
  }
}
