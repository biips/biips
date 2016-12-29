#include "distributions/DExp.hpp"

namespace Biips
{

  Bool DExp::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar lambda = paramValues[0].ScalarView();
    return lambda > 0.0;
  }

  DExp::MathDistType DExp::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar lambda = paramValues[0].ScalarView();

    return MathDistType(lambda);
  }

  DExp::RandomDistType DExp::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar lambda = paramValues[0].ScalarView();

    return RandomDistType(lambda);
  }

  Scalar DExp::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      Scalar lambda = paramValues[0].ScalarView();
      return std::log(lambda) - lambda * x;
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }
}
