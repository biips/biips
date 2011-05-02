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

  Bool DBin::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];
    return left.IsScalar() && right.IsScalar();
  }

  DimArray DBin::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }
  
  MultiArray DBin::Sample(const MultiArray::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    Scalar trials = paramValues[0].ScalarView(); // TODO check dim
    Scalar success_fraction = paramValues[1].ScalarView(); // TODO check dim

    typedef boost::binomial_distribution<Scalar> DistType;
    DistType dist(trials, success_fraction);

    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(pRng->GetGen(), dist);

    return MultiArray(gen());
  }


  Scalar DBin::LogPdf(const MultiArray & x, const MultiArray::Array & paramValues) const
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
