//                                               -*- C++ -*-
/*! \file MultiArray.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "common/MultiArray.hpp"
#include "common/Vector.hpp"
#include "common/Matrix.hpp"

namespace Biips
{

  MultiArray & MultiArray::operator = (const MultiArray & dat)
  {
    pDim_ = dat.DimPtr();
    pValues_ = dat.ValuesPtr();
    return *this;
  }


  MultiArray::MultiArray(const Vector & vec)
  : pDim_(new DimArray(1, vec.size())), pValues_(new StorageType(vec.data()))
  {

  }


  MultiArray::MultiArray(const Matrix & mat)
  : pDim_(new DimArray(2)), pValues_(new StorageType(mat.data()))
  {
    (*pDim_)[0] = mat.size1(); (*pDim_)[1] = mat.size2();
  }

  MultiArray & MultiArray::Alloc(const Vector & vec)
  {
    pDim_ = DimArray::Ptr(new DimArray(1,vec.size()));
    pValues_ = StorageType::Ptr(new StorageType(vec.data()));
    return *this;
  }
  MultiArray & MultiArray::Alloc(const Matrix & mat)
  {
    pDim_ = DimArray::Ptr(new DimArray(2));
    (*pDim_)[0] = mat.size1();
    (*pDim_)[1] = mat.size2();
    pValues_ = StorageType::Ptr(new StorageType(mat.data()));
    return *this;
  }


  VectorRef MultiArray::VectorView()
  {
    assert( IsVector() && pValues_);
    return VectorRef(*this);
  } // TODO throw exception

  MatrixRef MultiArray::MatrixView()
  {
    assert( IsMatrix() && pValues_ );
    return MatrixRef(*this);
  } // TODO throw exception




}
