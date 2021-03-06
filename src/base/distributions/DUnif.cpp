#include "distributions/DUnif.hpp"
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  Bool DUnif::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & lower = *paramDims[0];
    const DimArray & upper = *paramDims[1];
    return lower.IsScalar() && upper.IsScalar();
  }

  Bool DUnif::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar lower = paramValues[0].ScalarView();
    Scalar upper = paramValues[1].ScalarView();

    return lower < upper;
  }

  DimArray DUnif::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }

  void DUnif::sample(ValArray & values,
                     const NumArray::Array & paramValues,
                     const NumArray::Pair & boundValues,
                     Rng & rng) const
  {
    Scalar lower = paramValues[0].ScalarView();
    Scalar upper = paramValues[1].ScalarView();

    typedef boost::uniform_real<Scalar> DistType;
    DistType dist(lower, upper);
    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(rng.GetGen(), dist);

    values[0] = gen();
  }

  Scalar DUnif::logDensity(const NumArray & x,
                           const NumArray::Array & paramValues,
                           const NumArray::Pair & boundValues) const
  {
    Scalar lower = paramValues[0].ScalarView();
    Scalar upper = paramValues[1].ScalarView();
    Scalar my_point = x.ScalarView();

    if (my_point < lower || my_point > upper)
      return BIIPS_NEGINF;
    else
      return -std::log(upper - lower);
  }

  void DUnif::fixedUnboundedSupport(ValArray & lower,
                               ValArray & upper,
                               const NumArray::Array & paramValues) const
  {
    lower[0] = paramValues[0].ScalarView();
    upper[0] = paramValues[1].ScalarView();
  }

  Bool DUnif::IsSupportFixed(const Flags & fixmask) const
  {
    return fixmask[0] && fixmask[1]; //Lower and upper bounds fixed
  }
}
