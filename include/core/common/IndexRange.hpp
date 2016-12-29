/*! \file IndexRange.hpp
 * COPY: Adapted from JAGS Range class
 */

#ifndef BIIPS_INDEXRANGE_HPP_
#define BIIPS_INDEXRANGE_HPP_

#include "common/Types.hpp"
#include "common/DimArray.hpp"
#include <sstream>

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

    static DimArray::Ptr makeDim(const Types<IndexType>::Array & lower,
                                 const Types<IndexType>::Array & upper);

  protected:
    Indices lower_, upper_;
    DimArray::Ptr pDim_, pDimDropped_;
    SizeType length_;

    template<typename StorageOrderType>
    Types<IndexType>::Array getIndex(SizeType offset) const;

    template<typename StorageOrderType>
    SizeType getOffset(const Types<IndexType>::Array & indices) const;

  public:
    IndexRange() :
      lower_(0), upper_(0), pDim_(), pDimDropped_(), length_(0)
    {
    }

    IndexRange(const Types<IndexType>::Array & lower,
               const Types<IndexType>::Array & upper) :
      lower_(lower), upper_(upper), pDim_(makeDim(lower, upper)),
          pDimDropped_(new DimArray(pDim_->Drop())), length_(pDim_->Length())
    {
    }

    explicit IndexRange(const Types<IndexType>::Array & index) :
      lower_(index), upper_(index), pDim_(new DimArray(index.size(), 1)),
          pDimDropped_(new DimArray(pDim_->Drop())), length_(pDim_->Length())
    {
    }

    explicit IndexRange(const DimArray & dim) :
      lower_(dim.size(), 1), upper_(dim.begin(), dim.end()),
          pDim_(new DimArray(dim)), pDimDropped_(new DimArray(dim.Drop())),
          length_(dim.Length())
    {
    }

    explicit IndexRange(const DimArray::Ptr & pDim) :
      lower_(pDim->size(), 1), upper_(pDim->begin(), pDim->end()), pDim_(pDim),
          pDimDropped_(new DimArray(pDim->Drop())), length_(pDim_->Length())
    {
    }

    IndexRange(IndexType lower, IndexType upper) :
      lower_(1, lower), upper_(1, upper), pDim_(makeDim(lower_, upper_)),
          pDimDropped_(new DimArray(pDim_->Drop())), length_(pDim_->Length())
    {
    }

    explicit IndexRange(IndexType index) :
      lower_(1, index), upper_(1, index), pDim_(new DimArray(1, 1)),
          pDimDropped_(new DimArray(1, 1)), length_(1)
    {
    }

    explicit IndexRange(SizeType dim) :
      lower_(1, 1), upper_(1, dim), pDim_(new DimArray(1, dim)),
          pDimDropped_(new DimArray(pDim_->Drop())), length_(dim)
    {
    }

    SizeType NDim(Bool dropped = false) const
    {
      if (dropped)
        return pDimDropped_->NDim();
      else
        return pDim_->NDim();
    }
    const DimArray & Dim(Bool dropped = false) const
    {
      if (dropped)
        return *pDimDropped_;
      else
        return *pDim_;
    }
    const DimArray::Ptr & DimPtr(Bool dropped = false) const
    {
      if (dropped)
        return pDimDropped_;
      else
        return pDim_;
    }
    const Types<IndexType>::Array & Lower() const
    {
      return lower_;
    }
    const Types<IndexType>::Array & Upper() const
    {
      return upper_;
    }
    SizeType Length() const
    {
      return length_;
    }
    Bool IsNull() const
    {
      return length_ == 0;
    }

    Types<IndexType>::Array GetIndex(SizeType offset) const;

    SizeType GetOffset(const Types<IndexType>::Array & indices) const;

    Bool Contains(const IndexRange & range, Bool dropped = false) const;

    Bool Overlaps(const IndexRange & range) const;

    //! Equality operator
    Bool operator ==(const IndexRange & rhs) const;
    //! Inequality operator
    Bool operator !=(const IndexRange & rhs) const;

    Bool operator <(const IndexRange & rhs) const;
  };

  const IndexRange NULL_RANGE;

  /*!
   * Prints a BUGS language representation of the
   * given range: e.g. a range with lower limit (1,2,3) and upper limit
   * (3,3,3) will be printed as "[1:3,2:3,3]"
   */
  std::ostream & operator << (std::ostream & os, const IndexRange & range);
}
#endif /* BIIPS_INDEXRANGE_HPP_ */
