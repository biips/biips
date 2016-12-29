#include "distributions/DF.hpp"
#include <boost/math/special_functions/gamma.hpp>

namespace Biips
{

  Bool DF::CheckParamValues(const NumArray::Array & paramValues) const
  {
    return (paramValues[0].ScalarView() > 0.0) && (paramValues[1].ScalarView()
        > 0.0);
  }

  Bool DF::checkDensityParamValues(Scalar x,
                                   const NumArray::Array & paramValues) const
  {
    return x > 0.0 && CheckParamValues(paramValues);
  }

  DF::MathDistType DF::mathDist(const NumArray::Array & paramValues) const
  {
    return MathDistType(paramValues[0].ScalarView(),
                        paramValues[1].ScalarView());
  }

  DF::RandomDistType DF::randomDist(const NumArray::Array & paramValues) const
  {
    return RandomDistType(paramValues[0].ScalarView(),
                          paramValues[1].ScalarView());
  }

  Scalar DF::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      Scalar n1 = paramValues[0].ScalarView();
      Scalar n2 = paramValues[1].ScalarView();

      Scalar s = 0.5 * (n1 + n2);
      Scalar f = n1 / n2;

      using std::log;
      using boost::math::lgamma;

      return lgamma(s) - lgamma(0.5 * n1) - lgamma(0.5 * n2) + 0.5 * n1
          * log(f) + (0.5 * n1 - 1.0) * log(x) - s * log(1.0 + f * x);
    }

    MathDistType dist = mathDist(paramValues);

    return boost::math::pdf(dist, x);
  }

}
