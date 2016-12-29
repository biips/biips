/*! \file IndexRangeIterator.cpp
 * COPY: Adapted from JAGS RangeIterator class
 */

#include "common/IndexRangeIterator.hpp"

namespace Biips
{

  template<>
  IndexRangeIterator & IndexRangeIterator::next<ColumnMajorOrder>(Size increment)
  {
    SizeType n = range_.NDim();
    for (Size k=0; k<increment; ++k)
    {
      SizeType i = 0;
      for (; i < n; ++i)
      {
        IndexType & ind = operator[](i);
        if (ind < range_.Upper()[i])
        {
          ++ind;
          break;
        }
        else
          ind = range_.Lower()[i];
      }
      if (i == n)
        atend_++;
    }

    return *this;
  }


  template<>
  IndexRangeIterator & IndexRangeIterator::next<RowMajorOrder>(Size increment)
  {
    for (Size k=0; k<increment; ++k)
    {
      Int i = range_.NDim() - 1;
      for ( ; i >= 0; --i)
      {
        IndexType & ind = operator[](i);
        if (ind < range_.Upper()[i])
        {
          ++ind;
          break;
        }
        else
          ind = range_.Lower()[i];
      }
      if (i < 0)
        atend_++;
    }
    return *this;
  }

  IndexRangeIterator & IndexRangeIterator::Next(Size increment)
  {
    return next<StorageOrder>(increment);
  }

  Size IndexRangeIterator::AtEnd() const
  {
    return atend_;
  }
}
