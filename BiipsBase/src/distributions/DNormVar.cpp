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

  Bool DNormVar::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];
    return left.IsScalar() && right.IsScalar();
  }

  DimArray DNormVar::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }

  MultiArray DNormVar::Sample(const MultiArray::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    Scalar mean = paramValues[0].ScalarView(); // TODO check dim
    Scalar var = paramValues[1].ScalarView(); // TODO check dim

    typedef boost::math::normal DistType;
    DistType dist(mean, sqrt(var));

    using namespace boost::statistics::detail::distribution;
    typedef meta::random_generator<DistType, Rng::GenType&>::type GenType;
    GenType gen = make_random_generator(pRng->GetGen(), dist);

    return MultiArray(gen());
  }


  Scalar DNormVar::LogPdf(const MultiArray & x, const MultiArray::Array & paramValues) const
  {
    Scalar mean = paramValues[0].ScalarView(); // TODO check dim
    Scalar var = paramValues[1].ScalarView(); // TODO check dim
    Scalar my_point = x.ScalarView(); // TODO check dim

    typedef boost::math::normal DistType;
    DistType dist(mean, sqrt(var));
    return log(boost::math::pdf(dist, my_point));
//    return boost::math::log_unnormalized_pdf(dist, my_point);
  }

}
