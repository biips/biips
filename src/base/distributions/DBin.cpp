#include "distributions/DBin.hpp"
#include <boost/math/special_functions/binomial.hpp>

namespace Biips
{

  Bool DBin::CheckParamValues(const NumArray::Array & paramValues) const
  {
    Scalar success_fraction = paramValues[0].ScalarView();
    Scalar trials = paramValues[1].ScalarView();

    return checkSize(trials) && (success_fraction >= 0.0) && (success_fraction
        <= 1.0);
  }

  Bool DBin::checkDensityParamValues(Scalar x, const NumArray::Array & paramValues) const
  {
    Scalar success_fraction = paramValues[0].ScalarView();
    Scalar trials = paramValues[1].ScalarView();

    return checkSize(x) && checkSize(trials) && (success_fraction >= 0.0) && (success_fraction
        <= 1.0);
  }

  DBin::MathDistType DBin::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar success_fraction = paramValues[0].ScalarView();
    Scalar trials = paramValues[1].ScalarView();

    return MathDistType(trials, success_fraction);
  }

  DBin::RandomDistType DBin::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar success_fraction = paramValues[0].ScalarView();
    Scalar trials = paramValues[1].ScalarView();

    return RandomDistType(trials, success_fraction);
  }

  Scalar DBin::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      Scalar success_fraction = paramValues[0].ScalarView();
      Scalar trials = paramValues[1].ScalarView();
      using std::log;
      using boost::math::binomial_coefficient;
      return log(binomial_coefficient<Scalar> (roundSize(trials), roundSize(x)))
          + x * log(success_fraction) + (trials - x) * log(1.0
          - success_fraction);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }

  Scalar DBin::fixedUnboundedLower(const NumArray::Array & paramValues) const
  {
    return 0.0;
  }

  Scalar DBin::fixedUnboundedUpper(const NumArray::Array & paramValues) const
  {
    const Scalar trials = paramValues[1].ScalarView();
    return trials;
  }

  Bool DBin::IsSupportFixed(const Flags & fixmask) const
  {
    return fixmask[1]; //trials number is fixed;
  }

}
