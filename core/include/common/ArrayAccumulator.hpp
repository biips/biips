//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
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

/*! \file ArrayAccumulator.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_ARRAYACCUMULATOR_HPP_
#define BIIPS_ARRAYACCUMULATOR_HPP_

#include "common/MultiArray.hpp"
#include "common/Accumulator.hpp"

namespace Biips
{

  class ArrayAccumulator
  {
  public:
    typedef MultiArray::StorageType StorageType;

    typedef ArrayAccumulator SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef acc::accumulator_set<StorageType, acc::features<AccTags::Sum,
        AccTags::Mean, AccTags::Moment2, AccTags::Variance, AccTags::Moment3,
        AccTags::Skewness, AccTags::Moment4, AccTags::Kurtosis>, Scalar>
        AccType;

  protected:
    AccType acc_;
    DimArray::Ptr pDim_;

    std::map<StatTag, Bool> featuresMap_;

    void drop(StatTag tag);

  public:
    ArrayAccumulator();

    void ClearFeatures();
    void AddFeature(StatTag feat);
    void RemoveFeature(StatTag feat);

    void Init(const DimArray::Ptr & pDim);

    void Push(const StorageType & value, Scalar weight);

    Size Count() const;
    Scalar SumOfWeights() const;
    MultiArray Sum() const;
    MultiArray Mean() const;
    MultiArray Variance() const;
    MultiArray Skewness() const;
    MultiArray Kurtosis() const;
    template<Size Order>
    MultiArray Moment() const
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
      if (!pDim_)
        throw LogicError("Can not extract statistic: dim pointer is null.");
      ValArray::Ptr p_val(new ValArray(acc::weighted_moment<Order>(acc_)));
      return MultiArray(pDim_, p_val);
    }
  };
}

#endif /* BIIPS_ARRAYACCUMULATOR_HPP_ */
