//                                               -*- C++ -*-
/*! \file outputStream.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "print/outputStream.hpp"
#include "common/ValArray.hpp"
#include "common/MultiArray.hpp"
#include "common/IndexRange.hpp"

#include <sstream>

namespace Biips
{

  std::ostream & operator << (std::ostream & os, const ValArray & val)
  {
    if (!val.empty())
    {
      os << val[0];
      for (Size i = 1; i< val.size(); ++i)
        os << ", " << val[i];
      os << std::endl;
    }
    return os;
  }


  void printRecursion(std::ostream & os, const MultiArray & dat, const IndexRange & range, IndexRange::SizeType dim, IndexRange::Indices & indices)
  {
    typedef IndexRange::IndexType IndexType;
    typedef IndexRange::SizeType SizeType;
    typedef IndexRange::Indices Indices;

    if ( dim > 1 )
    {
      indices[dim] = range.Lower()[dim];
      while ( indices[dim] <= range.Upper()[dim] )
      {
        printRecursion(os, dat, range, dim-1, indices); // here is the recursion
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
          os << dat.Values()[range.GetOffset(indices)] << ", ";
          ++indices[dim];
        }
        os << dat.Values()[range.GetOffset(indices)];
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
        os << dat.Values()[range.GetOffset(indices)] << std::endl;
        ++indices[dim];
      }
      os << dat.Values()[range.GetOffset(indices)];
    }
  }


  std::ostream & operator << (std::ostream & os, const MultiArray & dat)
  {
    const IndexRange range(dat.Dim());
    Types<IndexRange::IndexType>::Array indices = range.Lower();
    printRecursion(os, dat, range, range.NDim()-1, indices);
    return os;
  }


  std::ostream & operator << (std::ostream & os, const IndexRange & range)
  {
    if (!range.IsNull())
    {
      os << "[";
      for (Size i = 0; i < range.NDim(false); ++i)
      {
        if (i > 0)
          os << ",";
        if (range.Lower()[i] == range.Upper()[i])
          os << range.Lower()[i];
        else
          os << range.Lower()[i] << ":" << range.Upper()[i];
      }
      os << "]";
    }
    return os;
  }
}
