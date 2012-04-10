//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file outStream.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "iostream/outStream.hpp"
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
          os << dat.Values()[range.GetOffset(indices)] << " ";
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
