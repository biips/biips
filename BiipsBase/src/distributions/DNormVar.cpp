//                                               -*- C++ -*-
/*! \file DNormVar.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DNormVar.hpp"
#include "rng/TruncatedNormal.hpp"

namespace Biips
{

  Bool DNormVar::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar var = paramValues[1].ScalarView();
    return isFinite(mean) && var > 0.0;
  }

  DNormVar::MathDistType DNormVar::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar var = paramValues[1].ScalarView();

    using std::sqrt;
    return MathDistType(mean, sqrt(var));
  }

  DNormVar::RandomDistType DNormVar::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar var = paramValues[1].ScalarView();

    using std::sqrt;
    return RandomDistType(mean, sqrt(var));
  }

  Scalar DNormVar::d(Scalar x,
                     const NumArray::Array & paramValues,
                     Bool give_log) const
  {
    if (give_log)
    {
      Scalar mean = paramValues[0].ScalarView();
      Scalar var = paramValues[1].ScalarView();
      using std::log;
      using std::pow;
      return -0.5 * (log(2 * M_PI) + log(var) + pow(x - mean, 2) / var);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }

  void DNormVar::sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar sigma = std::sqrt(paramValues[1].ScalarView());

    const NumArray & lower = boundValues.first;
    const NumArray & upper = boundValues.second;

    if (!lower.IsNULL() && !upper.IsNULL())
    {
      Scalar left = (lower.ScalarView() - mean) / sigma;
      Scalar right = (upper.ScalarView() - mean) / sigma;
      values[0] = mean + inormal(left, right, rng) * sigma;
    }
    else if (!lower.IsNULL())
    {
      Scalar left = (lower.ScalarView() - mean) / sigma;
      values[0] = mean + lnormal(left, rng) * sigma;
    }
    else if (!upper.IsNULL())
    {
      Scalar right = (upper.ScalarView() - mean) / sigma;
      values[0] = mean + rnormal(right, rng) * sigma;
    }
    else
      values[0] = r(paramValues, rng);
  }

}
