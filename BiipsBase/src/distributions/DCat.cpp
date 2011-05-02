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

  DimArray DCat::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *P_SCALAR_DIM;
  }

  MultiArray DCat::Sample(const MultiArray::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    const MultiArray & weights = paramValues[0]; // TODO check dim

    typedef boost::random::discrete_distribution<Int ,Scalar> GenType;
    GenType gen(weights.Values().begin(), weights.Values().end());

    return MultiArray(Scalar(gen(pRng->GetGen()) + 1));
  }


  Scalar DCat::LogPdf(const MultiArray & x, const MultiArray::Array & paramValues) const
  {
    const MultiArray & weights = paramValues[0]; // TODO check dim
    Scalar my_point = x.ScalarView(); // TODO check dim

    return log(weights.Values()[Size(my_point)-1]);
  }

}
