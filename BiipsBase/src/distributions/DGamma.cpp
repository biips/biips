//                                               -*- C++ -*-
/*! \file DGamma.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DGamma.hpp"
#include <boost/math/special_functions/gamma.hpp>

namespace Biips
{

  Bool DGamma::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar shape = paramValues[0].ScalarView();
    Scalar inv_scale = paramValues[1].ScalarView();
    return (shape > 0.0) && (inv_scale > 0.0);
  }

  DGamma::MathDistType DGamma::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar shape = paramValues[0].ScalarView();
    Scalar inv_scale = paramValues[1].ScalarView();

    return MathDistType(shape, 1.0 / inv_scale);
  }

  DGamma::RandomDistType DGamma::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar shape = paramValues[0].ScalarView();
    Scalar inv_scale = paramValues[1].ScalarView();

    return RandomDistType(shape, 1.0 / inv_scale);
  }

  Scalar DGamma::d(Scalar x, const NumArray::Array & paramValues, Bool give_log) const
  {
    if (x < 0.0)
      return give_log ? BIIPS_NEGINF : 0.0;

    if (give_log)
    {
      Scalar shape = paramValues[0].ScalarView();
      Scalar inv_scale = paramValues[1].ScalarView();
      using std::log;
      using boost::math::lgamma;
      return shape * log(inv_scale) + (shape - 1.0) * log(x) - inv_scale * x
          - lgamma(shape);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }
}
