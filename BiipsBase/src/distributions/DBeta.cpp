//                                               -*- C++ -*-
/*! \file DBeta.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include <boost/random/beta_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/math/distributions/beta.hpp>

#include "distributions/DBeta.hpp"

namespace Biips
{

  DataType DBeta::Sample(const DataType::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    Scalar alpha = paramValues[0].ScalarView(); // TODO check dim
    Scalar beta = paramValues[1].ScalarView(); // TODO check dim

    typedef boost::beta_distribution<Scalar> DistType;
    DistType dist(alpha, beta);

    typedef boost::variate_generator<Rng::GenType&, DistType > GenType;
    GenType gen(pRng->GetGen(), dist);

    return DataType(gen());
  }


  Scalar DBeta::LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const
  {
    // TODO check paramValues
    Scalar alpha = paramValues[0].ScalarView(); // TODO check dim
    Scalar beta = paramValues[1].ScalarView(); // TODO check dim
    Scalar my_point = x.ScalarView(); // TODO check dim

    typedef boost::math::beta_distribution<> DistType;
    DistType dist(alpha, beta);
    return log(boost::math::pdf(dist, my_point));
  }

}
