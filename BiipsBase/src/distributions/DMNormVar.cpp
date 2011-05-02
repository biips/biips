//                                               -*- C++ -*-
/*! \file DMNormVar.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#define _USE_MATH_DEFINES

#include <boost/random/multivariate_normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include "distributions/DMNormVar.hpp"
#include "common/cholesky.hpp"


namespace Biips
{

  Bool DMNormVar::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];
    if ((left.IsVector() && right.IsSquared()))
      return (left[0] == right[0]);
    else
      return false;
  }

  DimArray DMNormVar::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *paramDims[0];
  }

  MultiArray DMNormVar::Sample(const MultiArray::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    const MultiArray & mean = paramValues[0]; // TODO check dim
    const MultiArray & var = paramValues[1]; // TODO check dim

    Matrix var_chol(var);
    ublas::cholesky_factorize(var_chol);

    typedef boost::multivariate_normal_distribution<Scalar> DistType;

    DistType dist(var_chol, Vector(mean));

    typedef boost::variate_generator<Rng::GenType&, DistType > GenType;
    GenType gen(pRng->GetGen(), dist);

    MultiArray sample(mean.DimPtr());
    for (Size i = 0; i < sample.Length(); ++i)
    {
      sample.Values()[i] = gen();
    }
    return sample;
  }


  Scalar DMNormVar::LogPdf(const MultiArray & x, const MultiArray::Array & paramValues) const
  {
    // TODO check paramValues
    const MultiArray & mean = paramValues[0]; // TODO check dim
    const MultiArray & var = paramValues[1]; // TODO check dim

    Vector diff_vec(x.Length(), x.Values() - mean.Values()); // TODO check dim

    Matrix var_chol(var);
    ublas::cholesky_factorize(var_chol);

    ublas::inplace_solve(var_chol, diff_vec, ublas::lower_tag());
    return -log(ublas::cholesky_det(var_chol)) - 0.5 * (diff_vec.size()*log(2*M_PI) + ublas::inner_prod(diff_vec, diff_vec));
    //return -log(ublas::cholesky_det(var_chol)) - 0.5 * ublas::inner_prod(diff_vec, diff_vec);
  }


}
