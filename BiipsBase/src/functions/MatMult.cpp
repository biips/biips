//                                               -*- C++ -*-
/*! \file MatMult.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

//#include <boost/numeric/ublas/operation.hpp>

#include "functions/MatMult.hpp"

namespace Biips
{

  Bool MatMult::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];
    if ((left.IsMatrix() && right.IsVector()) || (left.IsMatrix()
        && right.IsMatrix()))
      return (left[1] == right[0]);
    else if (left.IsVector() && right.IsMatrix())
      return (left[0] == right[0]);
    else
      return false;
  }

  void MatMult::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    const NumArray & left = paramValues[0];
    const NumArray & right = paramValues[1];

    if (left.IsMatrix() && right.IsVector())
    {
      Vector ans_vec = ublas::prod(Matrix(left), Vector(right));
      values.swap(ans_vec.data());
    }
    else if (left.IsVector() && right.IsMatrix())
    {
      Vector ans_vec = ublas::prod(Vector(left), Matrix(right));
      values.swap(ans_vec.data());
    }
    else if (left.IsMatrix() && right.IsMatrix())
    {
      Matrix ans_mat = ublas::prod(Matrix(left), Matrix(right));
      values.swap(ans_mat.data());
    }
    else
      throw LogicError(String("Invalid dimensions in function ") + Name()
          + " evaluation.");

  }

  DimArray MatMult::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & left = *paramDims[0];
    const DimArray & right = *paramDims[1];

    DimArray dim;
    if (left.IsMatrix())
      dim.push_back(left[0]);
    if (right.IsMatrix())
      dim.push_back(right[1]);
    return dim.Drop();
  }

  Bool MatMult::IsScale(const Flags & scaleMask, const Flags & knownMask) const
  {
    return (knownMask[0] && scaleMask[1]) || (scaleMask[0] && knownMask[1]);
  }
}
