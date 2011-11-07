//                                               -*- C++ -*-
/*! \file DNorm.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DNorm.hpp"
#include "rng/TruncatedNormal.hpp"

namespace Biips
{

  Bool DNorm::checkParamValues(const MultiArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar prec = paramValues[1].ScalarView();
    return isFinite(mean) && prec > 0.0;
  }


  DNorm::MathDistType DNorm::mathDist(const MultiArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar prec = paramValues[1].ScalarView();

    using std::sqrt;
    return MathDistType(mean, 1.0/sqrt(prec));
  }


  DNorm::RandomDistType DNorm::randomDist(const MultiArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar prec = paramValues[1].ScalarView();

    using std::sqrt;
    return RandomDistType(mean, 1.0/sqrt(prec));
  }


  Scalar DNorm::d(Scalar x, const MultiArray::Array & paramValues,
      Bool give_log) const
  {
    if (give_log)
    {
      Scalar mean = paramValues[0].ScalarView();
      Scalar prec = paramValues[1].ScalarView();
      using std::log;
      using std::pow;
      return -0.5*(log(2*M_PI)-log(prec)+pow(x-mean, 2)*prec);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }


  MultiArray DNorm::sample(const MultiArray::Array & paramValues,
          const MultiArray::Pair & boundValues, Rng & rng) const
  {
    Scalar mean = paramValues[0].ScalarView();
    Scalar sigma = 1.0/std::sqrt(paramValues[1].ScalarView());

    const MultiArray & lower = boundValues.first;
    const MultiArray & upper = boundValues.second;

    if (!lower.IsNULL() && !upper.IsNULL())
    {
      Scalar left = (lower.ScalarView() - mean)/sigma;
      Scalar right = (upper.ScalarView() - mean)/sigma;
      return MultiArray(mean + inormal(left, right, rng) * sigma);
    }
    else if (!lower.IsNULL())
    {
      Scalar left = (lower.ScalarView() - mean)/sigma;
      return MultiArray(mean + lnormal(left, rng) * sigma);
    }
    else if (!upper.IsNULL())
    {
      Scalar right = (upper.ScalarView() - mean)/sigma;
      return MultiArray(mean + rnormal(right, rng) * sigma);
    }
    else
      return MultiArray(r(paramValues, rng));
  }
}
