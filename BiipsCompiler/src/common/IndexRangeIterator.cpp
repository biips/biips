//                                               -*- C++ -*-
/*! \file IndexRangeIterator.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Adapted from JAGS RangeIterator class
 */

#include "common/IndexRangeIterator.hpp"

namespace Biips
{

  IndexRangeIterator & IndexRangeIterator::Next(Size increment)
  {
    offset_ += increment;
    BaseType next(range_.GetIndex(offset_));
    assign(next.begin(), next.end());
    return *this;
  }

  Size IndexRangeIterator::AtEnd() const
  {
    Size len = range_.Length();
    return offset_ < len ? 0 : offset_+1-len;
  }
}
