#include "common/Vector.hpp"

namespace Biips
{

  Vector::Vector(const NumArray & data) :
    Vector::BaseType(data.Dim()[0], data.Values())
  {
    if (data.NDim() > 1)
      throw LogicError("Can not construct Vector: NumArray has more than 1 dimension.");
  }
  Vector::Vector(const MultiArray & data) :
    Vector::BaseType(data.Dim()[0], data.Values())
  {
    if (data.NDim() > 1)
      throw LogicError("Can not construct Vector: MultiArray has more than 1 dimension.");
  }

  VectorRef::VectorRef(VectorRef & vec_ref) :
    VectorRef::BaseType(vec_ref.size(), array_type()), pDim_(vec_ref.pDim_), pValues_(vec_ref.pValues_), released_(vec_ref.released_)
  {
    if (!released_)
    {
      swap(vec_ref);
      vec_ref.released_ = true;
    }
  }

  VectorRef::VectorRef(const VectorRef & vec_ref) :
    VectorRef::BaseType(vec_ref), pDim_(vec_ref.pDim_), pValues_(vec_ref.pValues_), released_(vec_ref.released_)
  {
    if (!released_)
    {
      vec_ref.released_ = true;
    }
  }

  void VectorRef::Release()
  {
    if (!released_)
    {
      (*pDim_)[0] = size();
      data().swap(*pValues_);
      released_ = true;
    }
  }

}

