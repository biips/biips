#include "distributions/DMulti.hpp"

#include <boost/random/discrete_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <algorithm>
#include <cmath>
#include <boost/math/special_functions/gamma.hpp>

using boost::math::lgamma;

namespace Biips
{

  Bool DMulti::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & weights_dim = *paramDims[0];
    const DimArray & size_dim = *paramDims[1];
    return weights_dim.Drop().IsVector() && (weights_dim[0] > 0) && size_dim.IsScalar() &&  (size_dim[0] > 0) ;
  }

  Bool DMulti::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const Scalar trials = paramValues[1].ScalarView();
    if (!checkSize(trials))
      return false;

    // If SIZE is non-zero, we need at least one non-zero probability
    Bool nz = trials == 0;

    const NumArray & weights = paramValues[0];

    for (Size i = 0; i < weights.Values().size(); ++i)
    {
      if (weights.Values()[i] < 0.0)
        return false;
      else if (weights.Values()[i] > 0.0)
        nz = true;
    }

    return nz;
  }


  Bool DMulti::CheckDensityParamValues(const NumArray & x,
                                       const NumArray::Array & paramValues) const
  {
    if (!CheckParamValues(paramValues))
      return false;

    const NumArray & weights = paramValues[0];
    const Size trials = roundSize(paramValues[1].ScalarView());

    Scalar n = 0.0;

    for (Size i=0; i<weights.Length(); ++i) {
      if (!checkSize(x.Values()[i])) {
        return false;
      }

      else if (x.Values()[i] != 0) {
        if (weights.Values()[i] == 0) {
          return false;
        }
        n += x.Values()[i];
      }
    }

    return n == trials;
  }

  DimArray DMulti::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *paramDims[0];
  }

  void DMulti::sample(ValArray & values,
                      const NumArray::Array & paramValues,
                      const NumArray::Pair & boundValues,
                      Rng & rng) const
  {
    const NumArray & weights = paramValues[0];
    const Size trials = roundSize(paramValues[1].ScalarView());

    typedef boost::random::discrete_distribution<Int, Scalar> DistType;
    DistType dist(weights.Values().begin(), weights.Values().end());
    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(rng.GetGen(), dist);

    std::fill(values.begin(), values.end(), 0);
    for (Size i = 0; i < trials; ++i) {
      values[gen()] += 1;
    }

  }

  Scalar DMulti::logDensity(const NumArray & x,
                            const NumArray::Array & paramValues,
                            const NumArray::Pair & boundValues) const
  {
    const NumArray & weights = paramValues[0];
    const Scalar trials = paramValues[1].ScalarView();

    Scalar log_dens = 0.0;
    for (Size i=0; i<weights.Length(); ++i) {
      if (x.Values()[i] != 0) {
        log_dens += x.Values()[i]*log(weights.Values()[i]);
        log_dens -= lgamma(x.Values()[i]+1);
      }
    }

    log_dens += lgamma(trials+1) - trials*log(weights.Values().Sum());

    return log_dens;
  }

  void DMulti::fixedUnboundedSupport(ValArray & lower,
                                     ValArray & upper,
                                     const NumArray::Array & paramValues) const
  {
    const NumArray & weights = paramValues[0];
    const Scalar trials = paramValues[1].ScalarView();
    for (Size i=0; i<weights.Length(); ++i) {
      lower[i] = 0;
      if (weights.Values()[i] == 0)
        upper[i] = 0;
      else
        upper[i] = trials;

    }
  }

}
