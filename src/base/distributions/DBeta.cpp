#include "distributions/DBeta.hpp"
#include "jrmath/JRmath.h"

#define ALPHA(paramValues) (paramValues[0].ScalarView())
#define BETA(paramValues) (paramValues[1].ScalarView())

namespace Biips
{

  Bool DBeta::CheckParamValues(const NumArray::Array & paramValues) const
  {
    return ALPHA(paramValues) > 0.0 && BETA(paramValues) > 0.0;
  }

  Bool DBeta::checkDensityParamValues(Scalar x,
                                      const NumArray::Array & paramValues) const
  {
    if (ALPHA(paramValues) > 0.0 && BETA(paramValues) > 0.0)
    {
      if (x == 0 && ALPHA(paramValues) < 1)
        return false;
      else if (x == 1 && BETA(paramValues) < 1)
        return false;
      return true;
    }
    return false;
  }

  DBeta::MathDistType DBeta::mathDist(const NumArray::Array & paramValues) const
  {
    return MathDistType(ALPHA(paramValues), BETA(paramValues));
  }

  DBeta::RandomDistType DBeta::randomDist(
      const NumArray::Array & paramValues) const
  {
    return RandomDistType(ALPHA(paramValues), BETA(paramValues));
  }

  Scalar DBeta::d(Scalar x, const NumArray::Array & paramValues,
                  Bool give_log) const
  {
    if (give_log)
    {
      return dbeta(x, ALPHA(paramValues), BETA(paramValues));
    }

    MathDistType dist = mathDist(paramValues);

    return boost::math::pdf(dist, x);
  }

  Scalar DBeta::r(const NumArray::Array & paramValues, Rng & rng) const
  {
    typedef boost::variate_generator<Rng::GenType&, RandomDistType> GenType;
    GenType gen(rng.GetGen(), randomDist(paramValues));

    Scalar ans = 0;
    // discard 0 and 1 values
    // because pdf not defined
    while (!(ans > 0 && ans < 1))
      ans = gen();
    return ans;
  }
}
