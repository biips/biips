//                                               -*- C++ -*-
/*! \file DMNorm.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#define _USE_MATH_DEFINES

#include <boost/random/multivariate_normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include "distributions/DMNorm.hpp"
#include "common/cholesky.hpp"


namespace Biips
{

  Bool DMNorm::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];
    if ((left.IsVector() && right.IsSquared()))
      return (left[0] == right[0]);
    else
      return false;
  }

  DimArray DMNorm::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *paramDims[0];
  }

  MultiArray DMNorm::Sample(const MultiArray::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    const MultiArray & mean = paramValues[0]; // TODO check dim
    const MultiArray & prec = paramValues[1]; // TODO check dim

    Matrix var_chol(prec);
    ublas::cholesky_factorize(var_chol);
    ublas::cholesky_invert(var_chol);
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


  Scalar DMNorm::LogUnnormPdf(const MultiArray & x, const MultiArray::Array & paramValues) const
  {
    // TODO check paramValues
    const MultiArray & mean = paramValues[0]; // TODO check dim
    const MultiArray & prec = paramValues[1]; // TODO check dim

    Vector diff_vec(x.Length(), x.Values() - mean.Values()); // TODO check dim

    Matrix prec_chol(prec);
    ublas::cholesky_factorize(prec_chol);

    diff_vec = ublas::prod(diff_vec, ublas::triangular_adaptor<Matrix,ublas::lower>(prec_chol));
    //return -1/2.0 * (diff_vec.size()*log(2*M_PI) + 2*log(choleskyDet(var_chol)) + ublas::inner_prod(diff_vec, diff_vec));
    return log(ublas::cholesky_det(prec_chol)) - ublas::inner_prod(diff_vec, diff_vec) / 2.0;
  }


}
