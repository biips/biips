//                                               -*- C++ -*-
/*! \file DChisqr.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DChisqr.hpp"
#include <boost/math/special_functions/gamma.hpp>

namespace Biips
{

  Bool DChisqr::checkParamValues(const MultiArray::Array & paramValues) const
  {
    Scalar degree = paramValues[0].ScalarView();
    return degree > 0.0;
  }


  DChisqr::MathDistType DChisqr::mathDist(const MultiArray::Array & paramValues) const
  {
    Scalar degree = paramValues[0].ScalarView();

    return MathDistType(degree);
  }


  DChisqr::RandomDistType DChisqr::randomDist(const MultiArray::Array & paramValues) const
  {
    Scalar degree = paramValues[0].ScalarView();

    return RandomDistType(degree);
  }


  Scalar DChisqr::d(Scalar x, const MultiArray::Array & paramValues,
      Bool give_log) const
  {
    if (x < 0.0)
      return give_log ? BIIPS_NEGINF : 0.0;

    if (give_log)
    {
      Scalar degree = paramValues[0].ScalarView();
      using std::log;
      using boost::math::tgamma;
      return 0.5*((degree-0.5)*log(x)-x-degree*log(2.0))-log(tgamma(0.5*degree));
    }

    MathDistType dist = mathDist(paramValues);

    using boost::math::pdf;
    return pdf(dist, x);
  }

}
