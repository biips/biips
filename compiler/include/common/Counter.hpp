//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
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

/*! \file Counter.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from JAGS Counter class
 */

#ifndef BIIPS_COUNTER_HPP_
#define BIIPS_COUNTER_HPP_

#include "common/IndexRangeIterator.hpp"

namespace Biips
{

  /*!
   * @short Mutable index that traverses a BUGS-language "for" loop
   *
   * The Counter class is used to represent counters in "for" loops in the
   * BUGS language. A Counter is a scalar RangeIterator.
   *
   * <pre>
   * for (i in 1:N) {
   * }
   * </pre>
   */
  class Counter : public IndexRangeIterator
  {
  public:
    typedef IndexRangeIterator BaseType;

    /*!
     * Create a counter
     * @param range Scalar range. i.e., Range having ndim(false) == 1
     */
    Counter(const IndexRange & range);
  };

}

#endif /* BIIPS_COUNTER_HPP_ */
