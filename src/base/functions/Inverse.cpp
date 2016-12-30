#include "functions/Inverse.hpp"
#include "common/cholesky.hpp"
#include "boost/numeric/ublas/io.hpp"

namespace Biips
{

  static const Scalar TOL = 1e-7;

  Bool Inverse::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & dim = *paramDims[0];

    return dim.Drop().IsSquared() || dim.IsScalar();
  }

  Bool Inverse::CheckParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & m = paramValues[0];


    // check symmetry
    Matrix m_mat(m);
    for (Size i = 1; i < m_mat.size1(); ++i)
    {
      for (Size j = 0; j < i; ++j)
      {
        if (std::fabs(m_mat(i, j) - m_mat(j, i)) > TOL) // TODO use epsilon
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
    {
      throw RuntimeError("Inverse::eval: matrix is not positive-semidefinite.");
    }
    ublas::cholesky_invert(m_inv);

    values.swap(m_inv.data());
  }

  DimArray Inverse::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & dim = *paramDims[0];

    return dim.Drop();
  }
}

