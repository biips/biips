#include <distributions/DInterval.hpp>

namespace Biips
{

  static Size value(Scalar t, const NumArray & cutpoints)
  {
    for (Size i = 0; i < cutpoints.Length(); ++i) {
      if (t <= cutpoints.Values()[i])
        return i;
    }
    return cutpoints.Length();
  }

  Bool DInterval::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return paramDims[0]->IsScalar();
  }

  Bool DInterval::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & cutpoints = paramValues[1];

    for (Size i=1; i<cutpoints.Length(); ++i)
    {
      if (cutpoints.Values()[i] <= cutpoints.Values()[i-1])
        return false;
    }
    return true;
  }

  DimArray DInterval::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }

  void DInterval::sample(ValArray & values,
                    const NumArray::Array & paramValues,
                    const NumArray::Pair & boundValues,
                    Rng & rng) const
  {
    /* sample for Dinterval is deterministic */
    const Scalar t = paramValues[0].ScalarView();
    const NumArray & cutpoints = paramValues[1];

    values[0] = static_cast<Scalar>(value(t, cutpoints));
  }

  Scalar DInterval::logDensity(const NumArray & x,
                          const NumArray::Array & paramValues,
                          const NumArray::Pair & boundValues) const
  {
    if (x.ScalarView() < 0)
      return BIIPS_NEGINF;

    Size x_int = static_cast<Size>(x.ScalarView());
    const NumArray & cutpoints = paramValues[1];

    if (x_int > cutpoints.Length()) {
      return BIIPS_NEGINF;
    }
    else {
      const Scalar t = paramValues[0].ScalarView();
      if (x_int > 0 && t <= cutpoints.Values()[x_int-1])
        return BIIPS_NEGINF;
      else if (x_int < cutpoints.Length() && t > cutpoints.Values()[x_int])
        return BIIPS_NEGINF;
      else
        return 0;
    }

  }

  void DInterval::fixedUnboundedSupport(ValArray & lower,
                              ValArray & upper,
                              const NumArray::Array & paramValues) const
  {
    const Scalar t = paramValues[0].ScalarView();
    const NumArray & cutpoints = paramValues[1];

    Size y = value(t, cutpoints);
    lower[0] = static_cast<Scalar>(y);
    upper[0] = static_cast<Scalar>(y);
  }

} /* namespace Biips */
