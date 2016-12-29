/*! \file IndexRangeIterator.hpp
 * COPY: Adapted from JAGS RangeIterator class
 */

#ifndef BIIPS_INDEXRANGEITERATOR_HPP_
#define BIIPS_INDEXRANGEITERATOR_HPP_

#include "common/IndexRange.hpp"

namespace Biips
{

  class IndexRangeIterator : public IndexRange::Indices
  {
  public:
    typedef IndexRangeIterator SelfType;
    typedef IndexRange::Indices BaseType;
    typedef BaseType::value_type IndexType;
    typedef BaseType::size_type SizeType;

    typedef IndexRange::StorageOrderType StorageOrder;
  protected:
    const IndexRange range_;
    Size atend_;

    template<typename StorageOrderType>
    IndexRangeIterator & next(Size increment = 1);

    //Forbid assignment
    IndexRangeIterator & operator=(const IndexRange::Indices &);

  public:
    explicit IndexRangeIterator(const IndexRange & range)
      : BaseType(range.GetIndex(0)), range_(range), atend_(0) {}

    IndexRangeIterator & Next(Size increment = 1);

    Size AtEnd() const;
  };

}

#endif /* BIIPS_INDEXRANGEITERATOR_HPP_ */
