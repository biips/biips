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

  Bool DBeta::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];
    return left.IsScalar() && right.IsScalar();
  }

  DimArray DBeta::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }

  MultiArray DBeta::Sample(const MultiArray::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    Scalar alpha = paramValues[0].ScalarView(); // TODO check dim
    Scalar beta = paramValues[1].ScalarView(); // TODO check dim

    typedef boost::beta_distribution<Scalar> DistType;
    DistType dist(alpha, beta);

    typedef boost::variate_generator<Rng::GenType&, DistType > GenType;
    GenType gen(pRng->GetGen(), dist);

    return MultiArray(gen());
  }


  Scalar DBeta::LogUnnormPdf(const MultiArray & x, const MultiArray::Array & paramValues) const
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
