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
    const DimArray & mean_dim = *paramDims[0];
    const DimArray & prec_dim = *paramDims[1];
    if ((mean_dim.Drop().IsVector() && prec_dim.IsSquared()))
      return (mean_dim[0] == prec_dim[0]);
    else
      return false;
  }

  Bool DMNorm::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const MultiArray & mean = paramValues[0];

    for (Size i=0; i<mean.Length(); ++i)
    {
      if (!isFinite(mean.Values()[i]))
        return false;
    }

    const MultiArray & prec = paramValues[1];

    // symmetric and positive
    Matrix prec_mat(prec);
    for(Size i=0; i<prec_mat.size1(); ++i)
    {
      if (prec_mat(i,i) <= 0.0)
        return false;
      for(Size j=0; j<i; ++j)
      {
        if (prec_mat(i,j) < 0.0 || prec_mat(i,j) != prec_mat(j,i))
          return false;
      }
    }
    // TODO check semi-definite positive

    return true;
  }

  DimArray DMNorm::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *paramDims[0];
  }

  MultiArray DMNorm::sample(const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues, Rng & rng) const
  {
    const MultiArray & mean = paramValues[0];
    const MultiArray & prec = paramValues[1];

    Matrix var_chol(prec);
    ublas::cholesky_factorize(var_chol);
    ublas::cholesky_invert(var_chol);
    ublas::cholesky_factorize(var_chol);

    typedef boost::multivariate_normal_distribution<Scalar> DistType;

    DistType dist(var_chol, Vector(mean));

    typedef boost::variate_generator<Rng::GenType&, DistType > GenType;
    GenType gen(rng.GetGen(), dist);

    MultiArray sample(mean.DimPtr());
    for (Size i = 0; i < sample.Length(); ++i)
    {
      sample.Values()[i] = gen();
    }
    return sample;
  }


  Scalar DMNorm::logDensity(const MultiArray & x, const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues) const
  {
    const MultiArray & mean = paramValues[0];
    const MultiArray & prec = paramValues[1];

    Vector diff_vec(x.Length(), x.Values() - mean.Values());

    Matrix prec_chol(prec);
    ublas::cholesky_factorize(prec_chol);

    diff_vec = ublas::prod(diff_vec, ublas::triangular_adaptor<Matrix,ublas::lower>(prec_chol));
    return log(ublas::cholesky_det(prec_chol)) - 0.5 * (diff_vec.size()*log(2*M_PI) + ublas::inner_prod(diff_vec, diff_vec));
  }


  MultiArray::Pair DMNorm::unboundedSupport(const MultiArray::Array & paramValues) const
  {
    const MultiArray & mean = paramValues[0];

    const ValArray::Ptr p_lower_val(new ValArray(mean.Length(), BIIPS_NEGINF));
    const ValArray::Ptr p_upper_val(new ValArray(mean.Length(), BIIPS_POSINF));

    static MultiArray::Pair support;
    support.first = MultiArray(mean.DimPtr(), p_lower_val);
    support.second = MultiArray(mean.DimPtr(), p_upper_val);

    return support;
  }
}
