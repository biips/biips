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


  Bool DMNormVar::checkParamValues(const MultiArray::Array & paramValues) const
  {
    const MultiArray & mean = paramValues[0];

    for (Size i=0; i<mean.Length(); ++i)
    {
      if (!isFinite(mean.Values()[i]))
        return false;
    }

    const MultiArray & var = paramValues[1];

    // symmetric and positive
    Matrix var_mat(var);
    for(Size i=0; i<var_mat.size1(); ++i)
    {
      if (var_mat(i,i) <= 0.0)
        return false;
      for(Size j=0; j<i; ++j)
      {
        if (var_mat(i,j) < 0.0 || var_mat(i,j) != var_mat(j,i))
          return false;
      }
    }
    // TODO check semi-definite positive

    return true;
  }

  MultiArray DMNormVar::sample(const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues, Rng & rng) const
  {
    const MultiArray & mean = paramValues[0];
    const MultiArray & var = paramValues[1];

    Matrix var_chol(var);
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


  Scalar DMNormVar::logDensity(const MultiArray & x, const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues) const
  {
    const MultiArray & mean = paramValues[0];
    const MultiArray & var = paramValues[1];

    Vector diff_vec(x.Length(), x.Values() - mean.Values());

    Matrix var_chol(var);
    ublas::cholesky_factorize(var_chol);

    ublas::inplace_solve(var_chol, diff_vec, ublas::lower_tag());
    return -log(ublas::cholesky_det(var_chol)) - 0.5 * (diff_vec.size()*log(2*M_PI) + ublas::inner_prod(diff_vec, diff_vec));
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
