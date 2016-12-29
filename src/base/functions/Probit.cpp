#include "functions/Probit.hpp"
#include <boost/math/distributions/normal.hpp>

namespace Biips
{


  Scalar ProbitScalar::operator()(Scalar val) const
  {
    boost::math::normal_distribution<Scalar> dist(0, 1);
    return boost::math::quantile(dist, val);
  }

  Scalar PhiScalar::operator()(Scalar val) const
  {
    boost::math::normal_distribution<Scalar> dist(0, 1);
    Scalar p = boost::math::cdf(dist, val);
    if (p == 0)
      return std::numeric_limits<Scalar>::epsilon();
    else if (p == 1)
      return 1 - std::numeric_limits<Scalar>::epsilon();
    else
      return p;
  }

  Bool Probit::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const ValArray & x = paramValues[0].Values();

    for (Size i = 0; i < x.size(); ++i)
    {
      if (x[i] <= 0.0 || x[i] >= 1)
        return false;
    }

    return true;
  }

} /* namespace Biips */
