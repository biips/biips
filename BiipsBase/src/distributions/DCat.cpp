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

  DataType DCat::Sample(const DataType::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    const DataType & weights = paramValues[0]; // TODO check dim

    typedef boost::random::discrete_distribution<Int ,Scalar> GenType;
    GenType gen(weights.Values().begin(), weights.Values().end());

    return DataType(Scalar(gen(pRng->GetGen()) + 1));
  }


  Scalar DCat::LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const
  {
    const DataType & weights = paramValues[0]; // TODO check dim
    Scalar my_point = x.ScalarView(); // TODO check dim

    return log(weights.Values()[Size(my_point)-1]);
  }

}
