//                                               -*- C++ -*-
/*! \file DUnif.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DUnif.hpp"
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  Bool DUnif::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & lower = *paramDims[0];
    const DimArray & upper = *paramDims[1];
    return lower.IsScalar() && upper.IsScalar();
  }


  Bool DUnif::checkParamValues(const MultiArray::Array & paramValues) const
  {
    Scalar lower = paramValues[0].ScalarView();
    Scalar upper = paramValues[1].ScalarView();

    return lower < upper;
  }


  DimArray DUnif::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }


  MultiArray DUnif::sample(const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues, Rng & rng) const
  {
    Scalar lower = paramValues[0].ScalarView();
    Scalar upper = paramValues[1].ScalarView();

    typedef boost::uniform_real<Scalar> DistType;
    DistType dist(lower, upper);
    typedef boost::variate_generator<Rng::GenType&, DistType> GenType;
    GenType gen(rng.GetGen(), dist);

    return MultiArray(gen());
  }


  Scalar DUnif::logDensity(const MultiArray & x, const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues) const
  {
    Scalar lower = paramValues[0].ScalarView();
    Scalar upper = paramValues[1].ScalarView();
    Scalar my_point = x.ScalarView();

    if (my_point < lower || my_point > upper)
        return BIIPS_NEGINF;
    else
        return -std::log(upper - lower);
  }


  MultiArray::Pair DUnif::unboundedSupport(const MultiArray::Array & paramValues) const
  {
    Scalar lower = paramValues[0].ScalarView();
    Scalar upper = paramValues[1].ScalarView();

    static MultiArray::Pair support;
    support.first = MultiArray(lower);
    support.second = MultiArray(upper);

    return support;
  }


  Bool DUnif::IsSupportFixed(const Flags & fixmask) const
  {
    return fixmask[0] && fixmask[1]; //Lower and upper bounds fixed
  }
}
