//                                               -*- C++ -*-
/*! \file Histogram.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
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
      positions[i] = it->first;
    return positions;
  }

  Types<Scalar>::Array Histogram::GetFrequencies() const
  {
    Types<Scalar>::Array freq(size());
    const_iterator it = begin();
    for (Size i = 0; it != end(); ++it, ++i)
      freq[i] = it->second;
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
    return it_max_value->first;
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
