//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file IndexRange.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from JAGS Range class
 */

#include "common/IndexRange.hpp"
#include "common/Error.hpp"
#include "common/Utility.hpp"

namespace Biips
{

  DimArray::Ptr IndexRange::makeDim(const Types<IndexType>::Array & lower,
                                    const Types<IndexType>::Array & upper)
  {
    Size ndim = upper.size();
    if (lower.size() != ndim)
      throw LogicError("IndexRange: Length mismatch in constructor");
    for (Size i = 0; i < ndim; i++)
    {
      if (upper[i] < lower[i])
        throw LogicError("IndexRange: upper < lower bound in constructor");
    }
    DimArray::Ptr p_dim(new DimArray(ndim));
    for (Size i = 0; i < lower.size(); ++i)
      (*p_dim)[i] = upper[i] - lower[i] + 1;
    return p_dim;
  }

  template<>
  Types<IndexRange::IndexType>::Array IndexRange::getIndex<RowMajorOrder>(SizeType offset) const
  {
    if (offset >= length_)
      throw LogicError("IndexRange::getIndex. Offset exceeds length of range");

    Types<IndexType>::Array indices(lower_);
    for (Int i = lower_.size() - 1; i >= 0; --i) // loop index is Int instead of Size because Size is unsigned int so --0 >= 0 (bad!)
    {
      indices[i] += offset % (*pDim_)[i];
      offset = offset / (*pDim_)[i];
    }
    return indices;
  }

  template<>
  Types<IndexRange::IndexType>::Array IndexRange::getIndex<ColumnMajorOrder>(SizeType offset) const
  {
    if (offset >= length_)
      throw LogicError("IndexRange::getIndex. Offset exceeds length of range");

    Types<IndexType>::Array indices(lower_);
    for (Size i = 0; i < lower_.size(); ++i)
    {
      indices[i] += offset % (*pDim_)[i];
      offset = offset / (*pDim_)[i];
    }
    return indices;
  }

  template<>
  IndexRange::SizeType IndexRange::getOffset<RowMajorOrder>(const Types<
      IndexType>::Array & indices) const
  {
    SizeType offset = 0;
    Size step = 1;
    for (Int i = indices.size() - 1; i >= 0; --i) // loop index is Int instead of Size because Size is unsigned int so --0 >= 0 (bad!)
    {
      if (indices[i] < lower_[i] || indices[i] > upper_[i])
        throw LogicError("IndexRange::getOffset. Index outside of allowed range");

      offset += step * (indices[i] - lower_[i]);
      step *= (*pDim_)[i];
    }
    return offset;
  }

  template<>
  IndexRange::SizeType IndexRange::getOffset<ColumnMajorOrder>(const Types<
      IndexType>::Array & indices) const
  {
    SizeType offset = 0;
    Size step = 1;
    for (Size i = 0; i < indices.size(); ++i)
    {
      if (indices[i] < lower_[i] || indices[i] > upper_[i])
        throw LogicError("IndexRange::getOffset. Index outside of allowed range");

      offset += step * (indices[i] - lower_[i]);
      step *= (*pDim_)[i];
    }
    return offset;
  }

  Types<IndexRange::IndexType>::Array IndexRange::GetIndex(SizeType offset) const
  {
    return getIndex<StorageOrderType> (offset);
  }

  IndexRange::SizeType IndexRange::GetOffset(const Types<IndexType>::Array & indices) const
  {
    return getOffset<StorageOrderType> (indices);
  }

  Bool IndexRange::Contains(const IndexRange & range, Bool dropped) const
  {
    if (range.NDim() > NDim() || range.NDim() < NDim(dropped))
      return false;
//      throw LogicError(String("IndexRange::Contains. Number of dimensions mismatch: ")
//          + print(NDim()) + " != " + print(range.NDim()) + "\nwhen checking "
//          + print(*this) + " contains " + print(range));
    for (Size i = 0; i < range.NDim(); ++i)
      if (range.lower_[i] < lower_[i] || range.upper_[i] > upper_[i])
        return false;
    return true;
  }

  Bool IndexRange::Overlaps(const IndexRange & range) const
  {
    if (NDim() != range.NDim())
      throw LogicError(String("IndexRange::Overlaps. Number of dimensions mismatch: ")
          + print(NDim()) + " != " + print(range.NDim()) + "\nwhen checking"
          + print(*this) + " overlaps " + print(range));
    for (Size i = 0; i < upper_.size(); ++i)
      if (range.upper_[i] < lower_[i] || range.lower_[i] > upper_[i])
        return false;
    return true;
  }

  Bool IndexRange::operator==(const IndexRange & rhs) const
  {
    return (lower_ == rhs.lower_) && (upper_ == rhs.upper_);
  }

  Bool IndexRange::operator!=(const IndexRange & rhs) const
  {
    return (lower_ != rhs.lower_) || (upper_ != rhs.upper_);
  }

  Bool IndexRange::operator<(const IndexRange & rhs) const
  {
    if (lower_ < rhs.lower_)
      return true;
    else if (rhs.lower_ < lower_)
      return false;
    else
      return upper_ < rhs.upper_;
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
