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

#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include "distributions/DMNormVar.hpp"
#include "common/cholesky.hpp"


namespace Biips
{

  Bool DMNormVar::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];
    if ((left.Drop().IsVector() && right.IsSquared()))
      return (left[0] == right[0]);
    else
      return false;
  }

  DimArray DMNormVar::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    return *paramDims[0];
  }


  Bool DMNormVar::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const MultiArray & mean = paramValues[0];

    for (Size i=0; i<mean.Length(); ++i)
    {
      if (!isFinite(mean.Values()[i]))
        return false;
    }

    const MultiArray & var = paramValues[1];

    // symmetric and positive diagonal
    Matrix var_mat(var);
    for(Size i=0; i<var_mat.size1(); ++i)
    {
      if (var_mat(i,i) <= 0.0)
        return false;
      for(Size j=0; j<i; ++j)
      {
        if (var_mat(i,j) != var_mat(j,i))
          return false;
      }
    }
    // TODO check positive-semidefinite

    return true;
  }

  MultiArray DMNormVar::sample(const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues, Rng & rng) const
  {
    const MultiArray & mean = paramValues[0];
    const MultiArray & var = paramValues[1];

    Size n_dim = mean.Values().size();

    Matrix var_chol(var);
    if (!ublas::cholesky_factorize(var_chol))
      throw RuntimeError("DMNormVar::sample: matrix is not positive-semidefinite.");

    typedef boost::normal_distribution<Scalar> DistType;
    boost::variate_generator<Rng::GenType&, DistType> gen(rng.GetGen(), DistType());

    MultiArray sample(mean.DimPtr());
    std::generate(sample.Values().begin(), sample.Values().end(), gen);

    VectorRef sample_vec(sample);
    sample_vec = ublas::prod(var_chol, sample_vec);
    sample_vec.Release();

    for (Size i=0; i<n_dim; ++i)
      sample.Values()[i] += mean.Values()[i];
    
    return sample;
  }


  Scalar DMNormVar::logDensity(const MultiArray & x, const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues) const
  {
    const MultiArray & mean = paramValues[0];
    const MultiArray & var = paramValues[1];

    Vector diff_vec(x.Length(), x.Values() - mean.Values());

    Matrix var_chol(var);
    if (!ublas::cholesky_factorize(var_chol))
      throw RuntimeError("DMNormVar::logDensity: matrix is not positive-semidefinite.");

    ublas::inplace_solve(var_chol, diff_vec, ublas::lower_tag());
    return -0.5 * (diff_vec.size()*std::log(2*M_PI) + std::log(ublas::cholesky_det(var_chol)) + ublas::inner_prod(diff_vec, diff_vec));
  }


  MultiArray::Pair DMNormVar::unboundedSupport(const MultiArray::Array & paramValues) const
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
