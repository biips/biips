//                                               -*- C++ -*-
/*! \file IndexRangeIterator.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
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

    typedef IndexRange::StorageOrder StorageOrder;
  protected:
    const IndexRange range_;

  public:
    explicit IndexRangeIterator(const IndexRange & range)
      : BaseType(range.GetIndex(0)), range_(range)
    {

    }

    IndexRangeIterator & Next(Size increment = 1);

    Bool AtEnd() const;
  };

}

#endif /* BIIPS_INDEXRANGEITERATOR_HPP_ */
