#include "distributions/DCat.hpp"

#include <boost/random/discrete_distribution.hpp>
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  Bool DCat::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & dim = *paramDims[0];
    if (dim.Drop().IsVector())
      return (dim[0] > 0);
    else
      return false;
  }

  Bool DCat::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Bool nz = false;
    const NumArray & weights = paramValues[0];

    for (Size i = 0; i < weights.Values().size(); ++i)
    {
      if (weights.Values()[i] < 0.0)
        return false;
      if (weights.Values()[i] > 0.0) {
          nz = true; //at least one non-zero probability
      }
    }

    return nz;
  }

  DimArray DCat::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }

  void DCat::sample(ValArray & values,
                    const NumArray::Array & paramValues,
                    const NumArray::Pair & boundValues,
                    Rng & rng) const
  {
    const NumArray & weights = paramValues[0];

    typedef boost::random::discrete_distribution<Int, Scalar> DistType;
    DistType dist(weights.Values().begin(), weights.Values().end());
    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(rng.GetGen(), dist);

    values[0] = Scalar(gen() + 1);
  }

  Scalar DCat::logDensity(const NumArray & x,
                          const NumArray::Array & paramValues,
                          const NumArray::Pair & boundValues) const
  {
    const NumArray & weights = paramValues[0];
    Scalar my_point = x.ScalarView();

    return log(weights.Values()[Size(my_point) - 1])
        - log(weights.Values().Sum());
  }

  void DCat::fixedUnboundedSupport(ValArray & lower,
                              ValArray & upper,
                              const NumArray::Array & paramValues) const
  {
    const NumArray & weights = paramValues[0];
    lower[0] = 1.0;
    upper[0] = Scalar(weights.Length());
  }

}
