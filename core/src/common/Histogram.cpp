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

/*! \file Histogram.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "common/Histogram.hpp"
#include "common/Integer.hpp"

namespace Biips
{

  Histogram::Histogram(const HistogramType & hist) :
    BaseType(hist.begin(), hist.end())
  {
  }

  Types<Scalar>::Array Histogram::GetPositions() const
  {
    Types<Scalar>::Array positions(size());
    const_iterator it = begin();
    for (Size i = 0; it != end(); ++it, ++i)
      positions[i] = Scalar(it->first);
    return positions;
  }

  Types<Scalar>::Array Histogram::GetFrequencies() const
  {
    Types<Scalar>::Array freq(size());
    const_iterator it = begin();
    for (Size i = 0; it != end(); ++it, ++i)
      freq[i] = Scalar(it->second);
    return freq;
  }

  struct ScalarPairSecondCompare
  {
    typedef Histogram::value_type PairType;
    Bool operator()(const PairType & lhs, const PairType & rhs) const
    {
      return lhs.second < rhs.second;
    }
  };

  Scalar DiscreteHistogram::MaxFreq() const
  {
    const_iterator it_max_value = std::max_element(begin(),
                                                   end(),
                                                   ScalarPairSecondCompare());
    return Scalar(it_max_value->first);
  }

  void DiscreteHistogram::Push(Scalar position, Scalar value)
  {
    Size u_position = roundSize(position);
    operator[](Scalar(u_position)) += value;
    normConst_ += value;
  }

  void DiscreteHistogram::Normalize()
  {
    if (normConst_ == 1.0 && normConst_ == 0.0)
      return;
    for (iterator it = begin(); it != end(); ++it)
      it->second /= normConst_;
    normConst_ = 1.0;
  }
}
