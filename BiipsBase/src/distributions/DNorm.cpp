//                                               -*- C++ -*-
/*! \file DNorm.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include <boost/statistics/detail/distribution_toolkit/distributions/normal/include.hpp>
#include <boost/statistics/detail/distribution_common/meta/random/generator.hpp>

#include "distributions/DNorm.hpp"

namespace Biips
{

  DataType DNorm::Sample(const DataType::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    Scalar mean = paramValues[0].ScalarView(); // TODO check dim
    Scalar prec = paramValues[1].ScalarView(); // TODO check dim

    typedef boost::math::normal DistType;
    DistType dist(mean, 1.0/sqrt(prec));

    using namespace boost::statistics::detail::distribution;
    typedef meta::random_generator<DistType, Rng::GenType&>::type GenType;
    GenType gen = make_random_generator(pRng->GetGen(), dist);

    return DataType(gen());
  }


  Scalar DNorm::LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView(); // TODO check dim
    Scalar prec = paramValues[1].ScalarView(); // TODO check dim
    Scalar my_point = x.ScalarView(); // TODO check dim

    typedef boost::math::normal DistType;
    DistType dist(mean, 1.0/sqrt(prec));
    return boost::math::log_unnormalized_pdf(dist, my_point);
  }

}
