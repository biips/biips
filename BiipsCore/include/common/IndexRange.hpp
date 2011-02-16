//                                               -*- C++ -*-
/*! \file IndexRange.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_INDEXRANGE_HPP_
#define BIIPS_INDEXRANGE_HPP_

#include "common/Types.hpp"
#include "common/DimArray.hpp"
#include "common/ValArray.hpp"

namespace Biips
{
  class IndexRange
  {
  public:
    typedef Int IndexType;
    typedef Size SizeType;
    typedef Types<IndexType>::Array Indices;

    typedef ValArray::StorageOrder StorageOrder;

    static DimArray makeDim(const Types<IndexType>::Array & lower, const Types<IndexType>::Array & upper);

    static DimArray makeDimDropped(const DimArray & dim);

  protected:
    Types<IndexType>::Array lower_, upper_;
    DimArray dim_, dimDropped_;
    SizeType length_;

    template<typename StorageOrderType>
    Types<IndexType>::Array getIndex (SizeType offset) const;

    template<typename StorageOrderType>
    SizeType getOffset (const Types<IndexType>::Array indices) const;

  public:
    IndexRange(const Types<IndexType>::Array & lower, const Types<IndexType>::Array & upper)
    : lower_(lower), upper_(upper),
      dim_(makeDim(lower, upper)), dimDropped_(makeDimDropped(dim_)),
      length_(dim_.Length()) {}

    explicit IndexRange(const Types<IndexType>::Array & index)
    : lower_(index), upper_(index),
      dim_(index.size(), 1), dimDropped_(makeDimDropped(dim_)),
      length_(dim_.Length()) {}

    explicit IndexRange(const DimArray & dim)
    : lower_(dim.size(), 1), upper_(dim.begin(), dim.end()),
      dim_(dim), dimDropped_(makeDimDropped(dim_)),
      length_(dim_.Length()) {}

    IndexRange(IndexType lower, IndexType upper)
    : lower_(1, lower), upper_(1, upper),
      dim_(makeDim(lower_, upper_)), dimDropped_(makeDimDropped(dim_)),
      length_(dim_.Length()) {}

    explicit IndexRange(IndexType index)
    : lower_(1, index), upper_(1, index),
      dim_(1, 1), dimDropped_(1, 1),
      length_(1) {}

    explicit IndexRange(SizeType dim)
    : lower_(1, 1), upper_(1, dim),
      dim_(1, dim), dimDropped_(makeDimDropped(dim_)),
      length_(dim) {}

    SizeType NDim(Bool dropped = false) const { if (dropped) return dimDropped_.NDim(); else return dim_.NDim(); }
    const Types<SizeType>::Array & Dim(Bool dropped = false) const { if (dropped) return dimDropped_; else return dim_; }
    const Types<IndexType>::Array & Lower() const { return lower_; }
    const Types<IndexType>::Array & Upper() const { return upper_; }
    SizeType Length() const { return length_; }


    Types<IndexType>::Array GetIndex (SizeType offset) const;

    SizeType GetOffset (const Types<IndexType>::Array indices) const;
  };
}
#endif /* BIIPS_INDEXRANGE_HPP_ */
