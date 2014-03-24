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

/*! \file Accumulator.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_ACCUMULATOR_HPP_
#define BIIPS_ACCUMULATOR_HPP_

#include <list>
#include <stdexcept>

#include "common/Types.hpp"
#include "common/Error.hpp"
#include "common/Histogram.hpp"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/weighted_sum.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>
#include <boost/accumulators/statistics/weighted_variance.hpp>
#include <boost/accumulators/statistics/weighted_moment.hpp>
#include <boost/accumulators/statistics/weighted_skewness.hpp>
#include <boost/accumulators/statistics/weighted_kurtosis.hpp>
#include <boost/accumulators/statistics/extended_p_square_quantile.hpp>
#include <boost/accumulators/statistics/weighted_density.hpp>

namespace acc = boost::accumulators;

namespace Biips
{

  namespace AccTags
  {
    typedef acc::tag::count Count;
    typedef acc::tag::sum_of_weights SumOfWeights;
    typedef acc::droppable<acc::tag::weighted_sum> Sum;
    typedef acc::droppable<acc::tag::weighted_mean> Mean;
    typedef acc::droppable<acc::tag::lazy_weighted_variance> Variance;
    typedef acc::droppable<acc::tag::weighted_moment<2> > Moment2;
    typedef acc::droppable<acc::tag::weighted_moment<3> > Moment3;
    typedef acc::droppable<acc::tag::weighted_moment<4> > Moment4;
    typedef acc::droppable<acc::tag::weighted_skewness> Skewness;
    typedef acc::droppable<acc::tag::weighted_kurtosis> Kurtosis;
    typedef acc::tag::weighted_density Density;
    typedef acc::tag::weighted_extended_p_square_quantile Quantiles;
  }

  class Accumulator
  {
  public:
    typedef Accumulator SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef acc::accumulator_set<LongScalar, acc::features<AccTags::Sum,
        AccTags::Mean, AccTags::Moment2, AccTags::Variance, AccTags::Moment3,
        AccTags::Skewness, AccTags::Moment4, AccTags::Kurtosis>, LongScalar>
        AccType;

  protected:
    std::map<StatTag, Bool> featuresMap_;

    AccType acc_;

    void drop(StatTag tag);

  public:
    Accumulator();

    void ClearFeatures();
    void AddFeature(StatTag feat);
    void RemoveFeature(StatTag feat);

    void Init();

    void Push(Scalar value, Scalar weight);

    Size Count() const;
    Scalar SumOfWeights() const;
    Scalar Sum() const;
    Scalar Mean() const;
    Scalar Variance() const;
    Scalar Skewness() const;
    Scalar Kurtosis() const;
    template<Size Order>
    Scalar Moment() const
    {
      switch (Order)
      {
        case 2:
          if (!featuresMap_.at(MOMENT2))
            throw LogicError("Invalid statistic extraction.");
          break;
        case 3:
          if (!featuresMap_.at(MOMENT3))
            throw LogicError("Invalid statistic extraction.");
          break;
        case 4:
          if (!featuresMap_.at(MOMENT4))
            throw LogicError("Invalid statistic extraction.");
          break;
        default:
          throw LogicError("Invalid moment order.");
          break;
      }
      return acc::weighted_moment<Order>(acc_);
    }
  };

  class DensityAccumulator
  {
  public:
    typedef DensityAccumulator SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef acc::accumulator_set<LongScalar, acc::features<AccTags::Density>,
        LongScalar> AccType;

  protected:
    Size cacheSize_;
    Size numBins_;
    AccType acc_;

  public:
    DensityAccumulator(Size cacheSize = 2, Size numBins = 1);

    void Init();

    void Push(Scalar value, Scalar weight);

    Size Count() const;
    Scalar SumOfWeights() const;
    Scalar Min() const;
    Scalar Max() const;
    Histogram Density() const;
  };

  class QuantileAccumulator
  {
  public:
    typedef QuantileAccumulator SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef acc::accumulator_set<LongScalar, acc::features<AccTags::Quantiles(acc::quadratic)>,
        LongScalar> AccType;

  protected:
    std::list<Scalar> quantileProbs_;
    Types<Size>::Array quantileRanks_;
    AccType acc_;

  public:
    QuantileAccumulator() :
      acc_(AccTags::Quantiles::probabilities = quantileProbs_)
    {
    }
    template<typename InputIterator>
    QuantileAccumulator(InputIterator first, InputIterator last) :
      quantileProbs_(first, last), quantileRanks_(quantileProbs_.size()), acc_(AccTags::Quantiles::probabilities
          = quantileProbs_)
    {
      // sort probs
      Types<Scalar>::Array prob_sorted(first, last);
      std::sort(prob_sorted.begin(), prob_sorted.end());

      // search ranks of probs in sorted vector
      Types<Size>::Iterator it_rank = quantileRanks_.begin();
      for (std::list<Scalar>::const_iterator it=quantileProbs_.begin();
          it!=quantileProbs_.end(); ++it, ++it_rank)
      {
        *it_rank = std::find(prob_sorted.begin(), prob_sorted.end(), *it) - prob_sorted.begin();
      }
    }

    void Init();

    void Push(Scalar value, Scalar weight);

    Size Count() const;
    Scalar SumOfWeights() const;
    Scalar Quantile(Size i) const;
  };

  class DiscreteAccumulator
  {
  public:
    typedef DiscreteAccumulator SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    DiscreteHistogram hist_;

  public:
    DiscreteAccumulator()
    {
    }

    void Init();

    void Push(Scalar value, Scalar weight);

    Scalar Min() const;
    Scalar Max() const;
    const DiscreteHistogram & Pdf();
    Scalar Mode() const;
  };

}

#endif /* BIIPS_ACCUMULATOR_HPP_ */
