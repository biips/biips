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

  DataType DMNormVar::Sample(const DataType::Array & paramValues, Rng * pRng) const
  {
    // TODO check paramValues
    const DataType & mean = paramValues[0]; // TODO check dim
    const DataType & var = paramValues[1]; // TODO check dim

    Matrix var_chol(var);
    ublas::cholesky_factorize(var_chol);

    typedef boost::multivariate_normal_distribution<Scalar> DistType;

    DistType dist(var_chol, Vector(mean));

    typedef boost::variate_generator<Rng::GenType&, DistType > GenType;
    GenType gen(pRng->GetGen(), dist);

    DataType sample(mean.DimPtr());
    for (Size i = 0; i < sample.Length(); ++i)
    {
      sample.Values()[i] = gen();
    }
    return sample;
  }


  Scalar DMNormVar::LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const
  {
    // TODO check paramValues
    const DataType & mean = paramValues[0]; // TODO check dim
    const DataType & var = paramValues[1]; // TODO check dim

    Vector diff_vec(x.Length(), x.Values() - mean.Values()); // TODO check dim

    Matrix var_chol(var);
    ublas::cholesky_factorize(var_chol);

    ublas::inplace_solve(var_chol, diff_vec, ublas::lower_tag());
    //return -1/2.0 * (diff_vec.size()*log(2*M_PI) + 2*log(choleskyDet(var_chol)) + ublas::inner_prod(diff_vec, diff_vec));
    return -log(ublas::cholesky_det(var_chol)) - ublas::inner_prod(diff_vec, diff_vec) / 2.0;
  }


}
