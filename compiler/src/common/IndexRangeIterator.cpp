//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file IndexRangeIterator.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
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
