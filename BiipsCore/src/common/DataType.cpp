//                                               -*- C++ -*-
/*! \file DataType.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "common/DataType.hpp"
#include "common/Vector.hpp"
#include "common/Matrix.hpp"

namespace Biips
{


  DataType & DataType::operator = (const DataType & dat)
  {
    pDim_ = dat.DimPtr();
    pValue_ = dat.ValuesPtr();
    return *this;
  }


  DataType::DataType(const Vector & vec)
  : pDim_(new DimArray(1, vec.size())), pValue_(new StorageType(vec.data()))
  {

  }


  DataType::DataType(const Matrix & mat)
  : pDim_(new DimArray(2)), pValue_(new StorageType(mat.data()))
  {
    (*pDim_)[0] = mat.size1(); (*pDim_)[1] = mat.size2();
  }

  DataType & DataType::Alloc(const Vector & vec)
  {
    pDim_ = DimArray::Ptr(new DimArray(1,vec.size()));
    pValue_ = StorageType::Ptr(new StorageType(vec.data()));
    return *this;
  }
  DataType & DataType::Alloc(const Matrix & mat)
  {
    pDim_ = DimArray::Ptr(new DimArray(2));
    (*pDim_)[0] = mat.size1();
    (*pDim_)[1] = mat.size2();
    pValue_ = StorageType::Ptr(new StorageType(mat.data()));
    return *this;
  }

  void DataType::printRecursion(std::ostream & os, const IndexRange & range, IndexRange::SizeType dim, IndexRange::Indices & indices) const
  {
    typedef IndexRange::IndexType IndexType;
    typedef IndexRange::SizeType SizeType;
    typedef IndexRange::Indices Indices;

    if ( dim > 1 )
    {
      indices[dim] = range.Lower()[dim];
      while ( indices[dim] <= range.Upper()[dim] )
      {
        printRecursion(os, range, dim-1, indices); // here is the recursion
        ++indices[dim];
      }
    }

    else if ( dim == 1 )
    {
      if ( indices.size() > 2 )
      {
        os << "(:, :";
        for (Size i = 2; i < indices.size(); ++i)
          os << ", " << indices[i];
        os << ")" << std::endl;
      }

      indices[dim-1] = range.Lower()[dim-1];
      while ( indices[dim-1] <= range.Upper()[dim-1] )
      {
        indices[dim] = range.Lower()[dim];
        while ( indices[dim] < range.Upper()[dim] )
        {
          os << Values()[range.GetOffset(indices)] << ", ";
          ++indices[dim];
        }
        os << Values()[range.GetOffset(indices)];
        if ( indices[dim-1] < range.Upper()[dim-1])
          os << std::endl;
        ++indices[dim-1];
      }
    }

    else if ( dim == 0 )
    {
      indices[dim] = range.Lower()[dim];
      while ( indices[dim] < range.Upper()[dim] )
      {
        os << Values()[range.GetOffset(indices)] << std::endl;
        ++indices[dim];
      }
      os << Values()[range.GetOffset(indices)];
    }
  }


  VectorRef DataType::VectorView()
  {
    assert( IsVector() && pValue_);
    return VectorRef(*this);
  } // TODO throw exception

  MatrixRef DataType::MatrixView()
  {
    assert( IsMatrix() && pValue_ );
    return MatrixRef(*this);
  } // TODO throw exception



  void DataType::Print(std::ostream & os) const
  {
    const IndexRange range = Range();
    Types<IndexRange::IndexType>::Array indices = range.Lower();
    printRecursion(os, range, range.NDim()-1, indices);
  }


  std::ostream & operator << (std::ostream & os, const DataType & dat)
  {
    dat.Print(os);
    return os;
  }


}
