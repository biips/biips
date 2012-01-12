//                                               -*- C++ -*-
/*! \file DPois.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DPois.hpp"

#define LAMBDA(par) (par[0].ScalarView())

namespace Biips
{
  Bool DPois::checkParamValues(const NumArray::Array & paramValues) const
  {
    return LAMBDA(paramValues) >= 0.0;
  }

  Bool DPois::checkDensityParamValues(Scalar x,
                                      const NumArray::Array & paramValues) const
  {
    return checkSize(x) && checkParamValues(paramValues);
  }

  DPois::MathDistType DPois::mathDist(const NumArray::Array & paramValues) const
  {
    return MathDistType(LAMBDA(paramValues));
  }

  DPois::RandomDistType DPois::randomDist(const NumArray::Array & paramValues) const
  {
    return RandomDistType(LAMBDA(paramValues));
  }

  Scalar DPois::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (give_log)
    {
      using std::log;
      Scalar log_d = -LAMBDA(paramValues) + x * log(LAMBDA(paramValues));
      for (Scalar i = 2.0; i - 0.5 < x; i += 1.0)
        log_d -= log(i);
      return log_d;
    }

    MathDistType dist = mathDist(paramValues);

    return boost::math::pdf(dist, x);
  }

}
