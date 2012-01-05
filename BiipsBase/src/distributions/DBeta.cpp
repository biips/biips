//                                               -*- C++ -*-
/*! \file DBeta.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "distributions/DBeta.hpp"

namespace Biips
{
  using std::log;

  Bool DBeta::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar beta = paramValues[1].ScalarView();

    return alpha > 0.0 && beta > 0.0;
  }

  Bool DBeta::checkDensityParamValues(Scalar x,
                                      const NumArray::Array & paramValues) const
  {
    if (checkParamValues(paramValues))
    {
      Scalar alpha = paramValues[0].ScalarView();
      Scalar beta = paramValues[1].ScalarView();
      return !(x == 0 && alpha < 1) && !(x == 1 && beta < 1);
    }
    else
      return false;
  }

  DBeta::MathDistType DBeta::mathDist(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar beta = paramValues[1].ScalarView();

    return MathDistType(alpha, beta);
  }

  DBeta::RandomDistType DBeta::randomDist(const NumArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar beta = paramValues[1].ScalarView();

    return RandomDistType(alpha, beta);
  }

}
