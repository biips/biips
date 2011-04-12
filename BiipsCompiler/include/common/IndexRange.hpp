//                                               -*- C++ -*-
/*! \file IndexRange.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Nearly copied and pasted from JAGS Range class
 */

#ifndef BIIPS_INDEXRANGE_HPP_
#define BIIPS_INDEXRANGE_HPP_

#include "common/Types.hpp"
#include "common/DimArray.hpp"

namespace Biips
{
  class IndexRange
  {
  public:
    typedef IndexRange SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Int IndexType;
    typedef Size SizeType;
    typedef Types<IndexType>::Array Indices;

    typedef StorageOrder StorageOrderType;

    static DimArray makeDim(const Types<IndexType>::Array & lower, const Types<IndexType>::Array & upper);

  protected:
    Indices lower_, upper_;
    DimArray dim_, dimDropped_; // TODO change into DimArray::Ptr ?
    SizeType length_;

    template<typename StorageOrderType>
    Types<IndexType>::Array getIndex (SizeType offset) const;

    template<typename StorageOrderType>
    SizeType getOffset (const Types<IndexType>::Array indices) const;

  public:
    IndexRange() : lower_(0), upper_(0), dim_(0), dimDropped_(0), length_(0) {}

    IndexRange(const Types<IndexType>::Array & lower, const Types<IndexType>::Array & upper)
    : lower_(lower), upper_(upper),
      dim_(makeDim(lower, upper)), dimDropped_(dim_.Drop()),
      length_(dim_.Length()) {}

    explicit IndexRange(const Types<IndexType>::Array & index)
    : lower_(index), upper_(index),
      dim_(index.size(), 1), dimDropped_(dim_.Drop()),
      length_(dim_.Length()) {}

    explicit IndexRange(const DimArray & dim)
    : lower_(dim.size(), 1), upper_(dim.begin(), dim.end()),
      dim_(dim), dimDropped_(dim_.Drop()),
      length_(dim_.Length()) {}

    IndexRange(IndexType lower, IndexType upper)
    : lower_(1, lower), upper_(1, upper),
      dim_(makeDim(lower_, upper_)), dimDropped_(dim_.Drop()),
      length_(dim_.Length()) {}

    explicit IndexRange(IndexType index)
    : lower_(1, index), upper_(1, index),
      dim_(1, 1), dimDropped_(1, 1),
      length_(1) {}

    explicit IndexRange(SizeType dim)
    : lower_(1, 1), upper_(1, dim),
      dim_(1, dim), dimDropped_(dim_.Drop()),
      length_(dim) {}

    SizeType NDim(Bool dropped = false) const { if (dropped) return dimDropped_.NDim(); else return dim_.NDim(); }
    const DimArray & Dim(Bool dropped = false) const { if (dropped) return dimDropped_; else return dim_; }
    const Types<IndexType>::Array & Lower() const { return lower_; }
    const Types<IndexType>::Array & Upper() const { return upper_; }
    SizeType Length() const { return length_; }
    Bool IsNull() const { return length_ == 0; }


    Types<IndexType>::Array GetIndex (SizeType offset) const;

    SizeType GetOffset (const Types<IndexType>::Array indices) const;

    // TODO throw exception
    Bool Contains(const IndexRange & range) const;

    //! Equality operator
    Bool operator == (const IndexRange & rhs) const;
    //! Inequality operator
    Bool operator != (const IndexRange & rhs) const;

    Bool operator < (const IndexRange & rhs) const;
  };

  const IndexRange NULL_RANGE;
}
#endif /* BIIPS_INDEXRANGE_HPP_ */
