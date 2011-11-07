//                                               -*- C++ -*-
/*! \file DWeib.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DWeib.hpp"

namespace Biips
{

  Bool DWeib::checkParamValues(const MultiArray::Array & paramValues) const
  {
    Scalar v = paramValues[0].ScalarView();
    Scalar lambda = paramValues[1].ScalarView();
    return (v > 0.0) && (lambda > 0.0);
  }


  DWeib::MathDistType DWeib::mathDist(const MultiArray::Array & paramValues) const
  {
    Scalar shape = paramValues[0].ScalarView();
    using std::pow;
    Scalar scale = pow(paramValues[1].ScalarView(), -1/shape);

    return MathDistType(shape, scale);
  }


  DWeib::RandomDistType DWeib::randomDist(const MultiArray::Array & paramValues) const
  {
    Scalar shape = paramValues[0].ScalarView();
    using std::pow;
    Scalar scale = pow(paramValues[1].ScalarView(), -1/shape);

    return RandomDistType(scale, shape);
  }


  Scalar DWeib::d(Scalar x, const MultiArray::Array & paramValues,
      Bool give_log) const
  {
    if (x == 0.0)
      return give_log ? BIIPS_NEGINF : 0.0;

    if (give_log)
    {
      Scalar v = paramValues[0].ScalarView();
      Scalar lambda = paramValues[1].ScalarView();
      using std::log;
      using std::pow;
      return log(v)+log(lambda)+(v-1)*log(x)-lambda*pow(x, v);
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }
}
