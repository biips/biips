//                                               -*- C++ -*-
/*! \file Histogram.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
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
