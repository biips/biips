//                                               -*- C++ -*-
/*! \file DDexp.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DDexp.hpp"

namespace Biips
{
  static inline Scalar MU(const NumArray::Array & par)
  {
    return par[0].ScalarView();
  }

  static inline Scalar RATE(const NumArray::Array & par)
  {
    return par[1].ScalarView();
  }

  DexpDistType::DexpDistType(Scalar mu, Scalar rate) :
    location_(mu), scale_(1.0 / rate), mathDist_(scale_),
        expRandomDist_(scale_)
  {
  }

  Bool DDexp::checkParamValues(const NumArray::Array & paramValues) const
  {
    return RATE(paramValues) > 0.0;
  }

  DDexp::MathDistType DDexp::mathDist(const NumArray::Array & paramValues) const
  {
    return MathDistType(MU(paramValues), RATE(paramValues));
  }

  DDexp::RandomDistType DDexp::randomDist(const NumArray::Array & paramValues) const
  {
    return RandomDistType(MU(paramValues), RATE(paramValues));
  }

  Scalar DDexp::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    MathDistType dist = mathDist(paramValues);

    if (give_log)
    {
      return boost::math::log_pdf(dist, x);
    }
    else
      return boost::math::pdf(dist, x);
  }
}
