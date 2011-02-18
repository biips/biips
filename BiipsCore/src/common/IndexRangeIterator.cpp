//                                               -*- C++ -*-
/*! \file IndexRangeIterator.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "common/IndexRangeIterator.hpp"

namespace Biips
{

  IndexRangeIterator & IndexRangeIterator::Next(Size increment)
  {
    Size current_offset = range_.GetOffset(*this);
    IndexRangeIterator next(range_.GetIndex(current_offset + increment));
    assign(next.begin(), next.end());
    return *this;
  }

  Bool IndexRangeIterator::AtEnd() const
  {
    Bool ans = true;
    for (Size i=0; i<size(); ++i)
    {
      if ((*this)[i] != range_.Upper()[i])
      {
        ans = false;
        break;
      }
    }
    return ans;
  }
}
