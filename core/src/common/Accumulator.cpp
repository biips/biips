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

/*! \file Accumulator.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "common/Accumulator.hpp"

namespace Biips
{

  Accumulator::Accumulator()
  {
    featuresMap_[SUM] = false;
    featuresMap_[MEAN] = false;
    featuresMap_[MOMENT2] = false;
    featuresMap_[VARIANCE] = false;
    featuresMap_[MOMENT3] = false;
    featuresMap_[SKEWNESS] = false;
    featuresMap_[MOMENT4] = false;
    featuresMap_[KURTOSIS] = false;
  }

  void Accumulator::drop(StatTag tag)
  {
    switch (tag)
    {
      case SUM:
        acc_.drop<AccTags::Sum>();
        break;
      case MEAN:
        acc_.drop<AccTags::Mean>();
        break;
      case MOMENT2:
        acc_.drop<AccTags::Moment2>();
        break;
      case VARIANCE:
        acc_.drop<AccTags::Variance>();
        break;
      case MOMENT3:
        acc_.drop<AccTags::Moment3>();
        break;
      case SKEWNESS:
        acc_.drop<AccTags::Skewness>();
        break;
      case MOMENT4:
        acc_.drop<AccTags::Moment4>();
        break;
      case KURTOSIS:
        acc_.drop<AccTags::Kurtosis>();
        break;
      default:
        throw LogicError("Invalid StatTag in Accumulator.");
        break;
    }
  }

  void Accumulator::ClearFeatures()
  {
    for (std::map<StatTag, Bool>::iterator it_feat = featuresMap_.begin();
        it_feat != featuresMap_.end(); ++it_feat)
      it_feat->second = false;
  }

  void Accumulator::Init()
  {
    acc_ = AccType();
    for (std::map<StatTag, Bool>::iterator it_feat = featuresMap_.begin();
        it_feat != featuresMap_.end(); ++it_feat)
      if (!it_feat->second)
        drop(it_feat->first);
  }

  void Accumulator::AddFeature(StatTag feat)
  {
    if (featuresMap_.find(feat) != featuresMap_.end())
      featuresMap_[feat] = true;
  }
  void Accumulator::RemoveFeature(StatTag feat)
  {
    if (featuresMap_.find(feat) != featuresMap_.end())
      featuresMap_[feat] = false;
  }

  void Accumulator::Push(Scalar value, Scalar weight)
  {
    acc_(value, acc::weight = weight);
  }

  Size Accumulator::Count() const
  {
    return acc::count(acc_);
  }
  Scalar Accumulator::SumOfWeights() const
  {
    return Scalar(acc::sum_of_weights(acc_));
  }
  Scalar Accumulator::Sum() const
  {
    if (!featuresMap_.at(SUM))
      throw LogicError("Invalid statistic extraction.");
    return Scalar(acc::weighted_sum(acc_));
  }
  Scalar Accumulator::Mean() const
  {
    if (!featuresMap_.at(MEAN))
      throw LogicError("Invalid statistic extraction.");
    return Scalar(acc::weighted_mean(acc_));
  }
  Scalar Accumulator::Variance() const
  {
    if (!featuresMap_.at(VARIANCE))
      throw LogicError("Invalid statistic extraction.");
    // FIXME: numerical precision problems here
    Scalar var(acc::weighted_variance(acc_));
    return var > 0.0 ? var : 0.0;
  }
  Scalar Accumulator::Skewness() const
  {
    if (!featuresMap_.at(SKEWNESS))
      throw LogicError("Invalid statistic extraction.");
    return Scalar(acc::weighted_skewness(acc_));
  }
  Scalar Accumulator::Kurtosis() const
  {
    if (!featuresMap_.at(KURTOSIS))
      throw LogicError("Invalid statistic extraction.");
    return Scalar(acc::weighted_kurtosis(acc_));
  }

  DensityAccumulator::DensityAccumulator(Size cacheSize, Size numBins)
      : cacheSize_(cacheSize), numBins_(numBins),
          acc_(AccTags::Density::cache_size = cacheSize,
               AccTags::Density::num_bins = numBins)
  {
  }

  void DensityAccumulator::Init()
  {
    acc_ = AccType(AccTags::Density::cache_size = cacheSize_,
                   AccTags::Density::num_bins = numBins_);
  }

  void DensityAccumulator::Push(Scalar value, Scalar weight)
  {
    acc_(value, acc::weight = weight);
  }

  Size DensityAccumulator::Count() const
  {
    return acc::count(acc_);
  }
  Scalar DensityAccumulator::SumOfWeights() const
  {
    return Scalar(acc::sum_of_weights(acc_));
  }
  Scalar DensityAccumulator::Min() const
  {
    return Scalar(acc::min(acc_));
  }
  Scalar DensityAccumulator::Max() const
  {
    return Scalar(acc::max(acc_));
  }
  Histogram DensityAccumulator::Density() const
  {
    return Histogram(acc::weighted_density(acc_));
  }

  void QuantileAccumulator::Init()
  {
    if (!quantileProbs_.empty())
    {
      quantileProbs_.sort();
      quantileProbs_.unique();
    }
    acc_ = AccType(AccTags::Quantiles::probabilities = quantileProbs_);
  }

  void QuantileAccumulator::Push(Scalar value, Scalar weight)
  {
    acc_(value, acc::weight = weight);
  }

  Size QuantileAccumulator::Count() const
  {
    return acc::count(acc_);
  }

  Scalar QuantileAccumulator::SumOfWeights() const
  {
    return Scalar(acc::sum_of_weights(acc_));
  }

  Scalar QuantileAccumulator::Quantile(Size i) const
  {
    if (i >= quantileProbs_.size())
      throw LogicError("Invalid quantile index.");

    return Scalar(acc::weighted_extended_p_square(acc_)[quantileRanks_[i]]);
  }

  void DiscreteAccumulator::Init()
  {
    hist_ = DiscreteHistogram();
  }

  void DiscreteAccumulator::Push(Scalar value, Scalar weight)
  {
    hist_.Push(value, weight);
  }

  Scalar DiscreteAccumulator::Min() const
  {
    return hist_.Min();
  }

  Scalar DiscreteAccumulator::Max() const
  {
    return hist_.Max();
  }

  Scalar DiscreteAccumulator::Mode() const
  {
    return hist_.MaxFreq();
  }

  const DiscreteHistogram & DiscreteAccumulator::Pdf()
  {
    hist_.Normalize();
    return hist_;
  }

}
