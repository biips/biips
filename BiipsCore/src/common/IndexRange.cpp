//                                               -*- C++ -*-
/*! \file IndexRange.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
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


  DimArray IndexRange::makeDimDropped(const DimArray & dim)
  {
    DimArray dimDropped;
    for (Size i = 0; i < dim.size(); ++i)
      if ( dim[i] != 1 )
        dimDropped.push_back(dim[i]);
    return (!dim.empty()) && dimDropped.empty() ? DimArray(1, 1) : dimDropped;
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
    return getIndex<StorageOrder>(offset);
  }

  IndexRange::SizeType IndexRange::GetOffset (const Types<IndexType>::Array indices) const
  {
    return getOffset<StorageOrder>(indices);
  }

}
