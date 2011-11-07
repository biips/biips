//                                               -*- C++ -*-
/*! \file DCat.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DCat.hpp"

#include <boost/random/discrete_distribution_sw_2009.hpp>
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  Bool DCat::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & dim = *paramDims[0];
    if (dim.IsVector())
      return (dim[0] > 0);
    else
      return false;
  }


  Bool DCat::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const MultiArray & weights = paramValues[0];

    for (Size i=0; i<weights.Values().size(); ++i)
    {
      if (weights.Values()[i] < 0.0)
        return false;
    }

    return true;
  }


  DimArray DCat::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }


  MultiArray DCat::sample(const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues, Rng & rng) const
  {
    const MultiArray & weights = paramValues[0];

    typedef boost::random::discrete_distribution<Int ,Scalar> DistType;
    DistType dist(weights.Values().begin(), weights.Values().end());
    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(rng.GetGen(), dist);

    return MultiArray(Scalar(gen() + 1));
  }


  Scalar DCat::logDensity(const MultiArray & x, const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues) const
  {
    const MultiArray & weights = paramValues[0];
    Scalar my_point = x.ScalarView();

    return log(weights.Values()[Size(my_point)-1]) - log(weights.Values().Sum());
  }


  MultiArray::Pair DCat::unboundedSupport(const MultiArray::Array & paramValues) const
  {
    const MultiArray & weights = paramValues[0];
    const Scalar n_cat(weights.Length());

    static const MultiArray lower(0.0);
    const MultiArray upper(n_cat);

    static MultiArray::Pair support;
    support.first = lower;
    support.second = upper;

    return support;
  }

}
