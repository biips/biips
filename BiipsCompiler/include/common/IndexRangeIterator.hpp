//                                               -*- C++ -*-
/*! \file IndexRangeIterator.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Nearly copied and pasted from JAGS RangeIterator class
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
    Size offset_;

  public:
    explicit IndexRangeIterator(const IndexRange & range)
      : BaseType(range.GetIndex(0)), range_(range), offset_(0)
    {
    }

    IndexRangeIterator & Next(Size increment = 1);

    Size AtEnd() const;
  };

}

#endif /* BIIPS_INDEXRANGEITERATOR_HPP_ */
