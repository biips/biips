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

  Bool DBeta::checkParamValues(const MultiArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar beta = paramValues[1].ScalarView();

    return alpha > 0.0 && beta > 0.0;
  }


  DBeta::MathDistType DBeta::mathDist(const MultiArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar beta = paramValues[1].ScalarView();

    return MathDistType(alpha, beta);
  }


  DBeta::RandomDistType DBeta::randomDist(const MultiArray::Array & paramValues) const
  {
    Scalar alpha = paramValues[0].ScalarView();
    Scalar beta = paramValues[1].ScalarView();

    return RandomDistType(alpha, beta);
  }

}
