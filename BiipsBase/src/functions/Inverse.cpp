//                                               -*- C++ -*-
/*! \file Inverse.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Inverse.hpp"
#include "common/cholesky.hpp"

namespace Biips
{

  Bool Inverse::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & dim = *paramDims[0];

    return dim.Drop().IsSquared() || dim.IsScalar();
  }

  Bool Inverse::checkParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & m = paramValues[0];

    Matrix m_mat(m);
    for (Size i = 1; i < m_mat.size1(); ++i)
    {
      for (Size j = 0; j < i; ++j)
      {
        if (m_mat(i, j) != m_mat(j, i)) // TODO use epsilon
          return false;
      }
    }
    return true;
  }

  void Inverse::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    const NumArray & m = paramValues[0];

    Matrix m_inv(m);
    if (!ublas::cholesky_factorize(m_inv))
      throw RuntimeError("Inverse::eval: matrix is not positive-semidefinite.");
    ublas::cholesky_invert(m_inv);

    values.swap(m_inv.data());
  }

  DimArray Inverse::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & dim = *paramDims[0];

    return dim.Drop();
  }
}

