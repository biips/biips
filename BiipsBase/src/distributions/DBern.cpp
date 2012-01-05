//                                               -*- C++ -*-
/*! \file DBern.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DBern.hpp"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  Bool DBern::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & p = *paramDims[0];
    return p.IsScalar();
  }

  Bool DBern::checkParamValues(const NumArray::Array & paramValues) const
  {
    Scalar p = paramValues[0].ScalarView();

    return (p > 0.0) && (p < 1.0);
  }

  DimArray DBern::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }

  void DBern::sample(ValArray & values,
                     const NumArray::Array & paramValues,
                     const NumArray::Pair & boundValues,
                     Rng & rng) const
  {
    Scalar p = paramValues[0].ScalarView();

    typedef boost::bernoulli_distribution<Scalar> DistType;
    DistType dist(p);
    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(rng.GetGen(), dist);

    values[0] = gen();
  }

  Scalar DBern::logDensity(const NumArray & x,
                           const NumArray::Array & paramValues,
                           const NumArray::Pair & boundValues) const
  {
    Scalar p = paramValues[0].ScalarView();
    Scalar my_point = x.ScalarView();

    using std::log;
    return my_point * log(p) + (1.0 - my_point) * log(1.0 - p);
  }

  void DBern::unboundedSupport(ValArray & lower,
                               ValArray & upper,
                               const NumArray::Array & paramValues) const
  {
    lower[0] = 0.0;
    upper[0] = 1.0;
  }

}
