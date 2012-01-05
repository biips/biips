//                                               -*- C++ -*-
/*! \file DPar.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DPar.hpp"

namespace Biips
{

  Bool DPar::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar c = paramValues[1].ScalarView();

    return (alpha >= 0.0) && (c <= 1.0);
  }

  DPar::MathDistType DPar::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar c = paramValues[1].ScalarView();

    return MathDistType(alpha, c);
  }

  DPar::RandomDistType DPar::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar c = paramValues[1].ScalarView();

    return RandomDistType(c, alpha);
  }

  Scalar DPar::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    Scalar c = paramValues[1].ScalarView();
    if (x < c)
      return give_log ? BIIPS_NEGINF : 0.0;

    if (give_log)
    {
      Scalar alpha = paramValues[0].ScalarView();
      using std::log;
      return log(alpha) + alpha * log(c) - (alpha + 1) * log(x);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }

  Scalar DPar::unboundedLower(const NumArray::Array & paramValues) const
  {
    Scalar c = paramValues[1].ScalarView();
    return c;
  }

  Scalar DPar::unboundedUpper(const NumArray::Array & paramValues) const
  {
    return BIIPS_POSINF;
  }

  Bool DPar::IsSupportFixed(const Flags & fixmask) const
  {
    return fixmask[1]; //Fixed if C is fixed
  }

}
