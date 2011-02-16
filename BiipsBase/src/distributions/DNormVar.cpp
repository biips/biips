//                                               -*- C++ -*-
/*! \file DNormVar.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include <boost/statistics/detail/distribution_toolkit/distributions/normal/include.hpp>
#include <boost/statistics/detail/distribution_common/meta/random/generator.hpp>

#include "distributions/DNormVar.hpp"

namespace Biips
{

  DataType DNormVar::Sample(const DataType::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    Scalar mean = paramValues[0].ScalarView(); // TODO check dim
    Scalar var = paramValues[1].ScalarView(); // TODO check dim

    typedef boost::math::normal DistType;
    DistType dist(mean, sqrt(var));

    using namespace boost::statistics::detail::distribution;
    typedef meta::random_generator<DistType, Rng::GenType&>::type GenType;
    GenType gen = make_random_generator(pRng->GetGen(), dist);

    return DataType(gen());
  }


  Scalar DNormVar::LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView(); // TODO check dim
    Scalar var = paramValues[1].ScalarView(); // TODO check dim
    Scalar my_point = x.ScalarView(); // TODO check dim

    typedef boost::math::normal DistType;
    DistType dist(mean, sqrt(var));
    return boost::math::log_unnormalized_pdf(dist, my_point);
  }

}
