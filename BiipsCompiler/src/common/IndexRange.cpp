//                                               -*- C++ -*-
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

namespace Biips
{

  DimArray IndexRange::makeDim(const Types<IndexType>::Array & lower, const Types<IndexType>::Array & upper)
  {
    // TODO check arrays
    DimArray dim(lower.size());
    for (Size i = 0; i < lower.size(); ++i)
      dim[i] = upper[i] - lower[i] + 1;
    return dim;
  }


  template<>
  Types<IndexRange::IndexType>::Array IndexRange::getIndex<RowMajorOrder> (SizeType offset) const
  {
    Types<IndexType>::Array indices(lower_);
    for (Int i = lower_.size()-1; i >= 0; --i) // loop index is Int instead of Size because Size is unsigned int so --0 >= 0 (bad!)
    {
      indices[i] += offset % dim_[i];
      offset = offset / dim_[i];
    }
    return indices;
  }

  template<>
  Types<IndexRange::IndexType>::Array IndexRange::getIndex<ColumnMajorOrder> (SizeType offset) const
  {
    Types<IndexType>::Array indices(lower_);
    for (Size i = 0; i < lower_.size(); ++i)
    {
      indices[i] += offset % dim_[i];
      offset = offset / dim_[i];
    }
    return indices;
  }


  template<>
  IndexRange::SizeType IndexRange::getOffset<RowMajorOrder> (const Types<IndexType>::Array indices) const
  {
    SizeType offset = 0;
    Size step = 1;
    for (Int i = indices.size()-1; i >= 0; --i) // loop index is Int instead of Size because Size is unsigned int so --0 >= 0 (bad!)
    {
      offset += step * (indices[i] - lower_[i]);
      step *= dim_[i];
    }
    return offset;
  }

  template<>
  IndexRange::SizeType IndexRange::getOffset<ColumnMajorOrder> (const Types<IndexType>::Array indices) const
  {
    SizeType offset = 0;
    Size step = 1;
    for (Size i = 0; i < indices.size(); ++i)
    {
      offset += step * (indices[i] - lower_[i]);
      step *= dim_[i];
    }
    return offset;
  }



  Types<IndexRange::IndexType>::Array IndexRange::GetIndex (SizeType offset) const
  {
    return getIndex<StorageOrderType>(offset);
  }

  IndexRange::SizeType IndexRange::GetOffset (const Types<IndexType>::Array indices) const
  {
    return getOffset<StorageOrderType>(indices);
  }


  Bool IndexRange::Contains(const IndexRange & range) const
  {
    for (Size i = 0; i < upper_.size(); ++i)
      if (range.lower_[i] < lower_[i] || range.upper_[i] > upper_[i])
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
}
