#include "distributions/DLnorm.hpp"

namespace Biips
{

  using std::sqrt;
  using std::log;
  using std::pow;

  Bool DLnorm::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar tau = paramValues[1].ScalarView();
    return tau > 0.0;
  }

  DLnorm::MathDistType DLnorm::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar mu = paramValues[0].ScalarView();
    Scalar tau = paramValues[1].ScalarView();
    using std::sqrt;
    return MathDistType(mu, 1.0 / sqrt(tau));
  }

  DLnorm::RandomDistType DLnorm::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar mu = paramValues[0].ScalarView();
    Scalar tau = paramValues[1].ScalarView();
    using std::sqrt;
    return RandomDistType(mu, 1.0 / sqrt(tau));
  }

  Scalar DLnorm::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      Scalar mu = paramValues[0].ScalarView();
      Scalar tau = paramValues[1].ScalarView();
      using std::log;
      using std::pow;
      return 0.5 * log(tau) - log(x) - 0.5 * tau * pow(log(x) - mu, 2);
    }

    MathDistType dist = mathDist(paramValues);

    using namespace boost::math;
    return pdf(dist, x);
  }
}
