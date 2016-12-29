#include "common/Matrix.hpp"

namespace Biips
{

  Matrix::Matrix(const NumArray & data) :
      Matrix::BaseType(data.Dim()[0],
                       data.IsVector() ? 1 : data.Dim()[1],
                       data.Values())
  {
    if (data.NDim() > 2)
      throw LogicError("Can not construct Matrix: NumArray has more than 2 dimensions.");
  }
  Matrix::Matrix(const MultiArray & data) :
      Matrix::BaseType(data.Dim()[0],
                       data.IsVector() ? 1 : data.Dim()[1],
                       data.Values())
  {
    if (data.NDim() > 2)
      throw LogicError("Can not construct Matrix: NumArray has more than 2 dimensions.");
  }

  MatrixRef::MatrixRef(MatrixRef & mat_ref) :
      MatrixRef::BaseType(mat_ref.size1(), mat_ref.size2(), array_type()), pDim_(mat_ref.pDim_), pValues_(mat_ref.pValues_), released_(mat_ref.released_)
  {
    if (!released_)
    {
      swap(mat_ref);
      mat_ref.released_ = true;
    }
  }

  MatrixRef::MatrixRef(const MatrixRef & mat_ref) :
      MatrixRef::BaseType(mat_ref), pDim_(mat_ref.pDim_), pValues_(mat_ref.pValues_), released_(mat_ref.released_)
  {
    if (!released_)
      mat_ref.released_ = true;
  }

  void MatrixRef::Release()
  {
    if (!released_)
    {
      (*pDim_)[0] = BaseType::size1();
      (*pDim_)[1] = BaseType::size2();
      data().swap(*pValues_);
      released_ = true;
    }
  }

}

