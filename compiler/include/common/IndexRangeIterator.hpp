//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file IndexRangeIterator.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate: 2011-02-28 18:15:08 +0100 (lun., 28 févr. 2011) $
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
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
