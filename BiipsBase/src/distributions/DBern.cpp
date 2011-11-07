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


  Bool DBern::checkParamValues(const MultiArray::Array & paramValues) const
  {
    Scalar p = paramValues[0].ScalarView();

    return (p > 0.0) && (p < 1.0);
  }


  DimArray DBern::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }


  MultiArray DBern::sample(const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues, Rng & rng) const
  {
    Scalar p = paramValues[0].ScalarView();

    typedef boost::bernoulli_distribution<Scalar> DistType;
    DistType dist(p);
    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(rng.GetGen(), dist);

    return MultiArray(gen());
  }


  Scalar DBern::logDensity(const MultiArray & x, const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues) const
  {
    Scalar p = paramValues[0].ScalarView();
    Scalar my_point = x.ScalarView();

    using std::log;
    return my_point * log(p) + (1.0-my_point) * log(1.0-p);
  }


  MultiArray::Pair DBern::unboundedSupport(const MultiArray::Array & paramValues) const
  {
    static const MultiArray lower(0.0);
    static const MultiArray upper(1.0);

    static MultiArray::Pair support;
    support.first = lower;
    support.second = upper;

    return support;
  }

}
