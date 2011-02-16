//                                               -*- C++ -*-
/*! \file DBin.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include <boost/random/binomial_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/math/distributions/binomial.hpp>

#include "distributions/DBin.hpp"

namespace Biips
{
  
  DataType DBin::Sample(const DataType::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    Scalar trials = paramValues[0].ScalarView(); // TODO check dim
    Scalar success_fraction = paramValues[1].ScalarView(); // TODO check dim

    typedef boost::binomial_distribution<Scalar> DistType;
    DistType dist(trials, success_fraction);

    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(pRng->GetGen(), dist);

    return DataType(gen());
  }


  Scalar DBin::LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const
  {
    // TODO check paramValues
    Scalar trials = paramValues[0].ScalarView(); // TODO check dim
    Scalar success_fraction = paramValues[1].ScalarView(); // TODO check dim
    Scalar my_point = x.ScalarView(); // TODO check dim

    typedef boost::math::binomial DistType;
    DistType dist(trials, success_fraction);

    return log(boost::math::pdf(dist, my_point));
  }

}
