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

/*! \file Histogram.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_HISTOGRAM_HPP_
#define BIIPS_HISTOGRAM_HPP_

#include <map>
#include <boost/range/iterator_range.hpp>
#include "common/Types.hpp"

namespace Biips
{

  class Histogram: public std::map<Scalar, Scalar>
  {
  protected:
    typedef std::map<Scalar, Scalar> BaseType;
    typedef boost::iterator_range<Types<Types<Scalar>::Pair>::Iterator>
        HistogramType;

  public:
    Histogram()
    {
    }
    Histogram(const HistogramType & hist);

    Types<Scalar>::Array GetPositions() const;
    Types<Scalar>::Array GetFrequencies() const;
    Scalar Min() const
    {
      return begin()->first;
    }
    Scalar Max() const
    {
      return (--end())->first;
    }
  };

  class DiscreteHistogram: public Histogram
  {
  public:
    typedef Histogram BaseType;

  protected:
    Scalar normConst_;

  public:
    DiscreteHistogram()
    {
    }
    Scalar MaxFreq() const;

    void Push(Scalar position, Scalar value);
    void Normalize();
  };

}

#endif /* BIIPS_HISTOGRAM_HPP_ */
