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

/*! \file Monitor.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MONITOR_HPP_
#define BIIPS_MONITOR_HPP_

#include "common/Types.hpp"
#include "common/ValArray.hpp"
#include "common/DimArray.hpp"
#include <map>

namespace Biips
{

  class Accumulator;
  class DensityAccumulator;
  class QuantileAccumulator;
  class DiscreteAccumulator;
  class ArrayAccumulator;
  class Particle;

  class Monitor
  {
  public:
    typedef Monitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    Size iter_;
    //    ValArray logWeights_;
    ValArray weights_;
    Scalar ess_;
    Scalar sumOfWeights_;
    Types<NodeId>::Array sampledNodes_;
    typedef Types<ValArray::Ptr>::Array ParticleValues;
    std::map<NodeId, ParticleValues> particleValuesMap_;
    std::map<NodeId, Size> nodeIterationMap_;
    std::map<Size, Scalar> iterationEssMap_;
    std::map<NodeId, Bool> nodeDiscreteMap_;

    Bool weightsSet_;
    Bool weightsSwapped_;
    //    Bool logWeightsSwapped_;
    void checkWeightsSet() const;
    void checkWeightsSwapped() const;
    //    void checkLogWeightsSwapped() const;

  public:
    Monitor(Size iter, const Types<NodeId>::Array & sampledNodes) :
      iter_(iter), sampledNodes_(sampledNodes), weightsSet_(false),
          weightsSwapped_(false)/*, logWeightsSwapped_(false)*/
    {
    }
    virtual ~Monitor()
    {
    }

    Bool Contains(NodeId nodeId) const;
    Types<NodeId>::Array GetNodes() const;

    Size NParticles() const
    {
      checkWeightsSet();
      checkWeightsSwapped();
      return weights_.size();
    }
    Size GetIteration() const
    {
      return iter_;
    }
    const Types<NodeId>::Array & GetSampledNodes() const
    {
      return sampledNodes_;
    }
    Scalar GetESS() const
    {
      checkWeightsSet();
      return ess_;
    }
    Size GetNodeSamplingIteration(NodeId id) const;
    Bool HasIterationESS(Size iter) const;
    void SetIterationESS(Size iter, Scalar ess);
    Scalar GetNodeESS(NodeId id) const;
    Bool GetNodeDiscrete(NodeId id) const;
    Scalar GetSumOfWeights() const
    {
      checkWeightsSet();
      return sumOfWeights_;
    }
    //    const ValArray & GetUnnormLogWeights() const
    //    {
    //      checkWeightsSet();
    //      checkLogWeightsSwapped();
    //      return logWeights_;
    //    }
    const ValArray & GetUnnormWeights() const
    {
      checkWeightsSet();
      checkWeightsSwapped();
      return weights_;
    }
    ValArray GetWeights() const
    {
      checkWeightsSet();
      checkWeightsSwapped();
      return weights_ / sumOfWeights_;
    }
    Bool WeightsSet() const
    {
      return weightsSet_;
    }

    //    void SwapLogWeights(ValArray::BaseType & vec)
    //    {
    //      logWeights_.swap(vec);
    //      logWeightsSwapped_ = !logWeightsSwapped_;
    //    }

    void SwapWeights(ValArray::BaseType & vec)
    {
      weights_.swap(vec);
      weightsSwapped_ = !weightsSwapped_;
    }

    const ParticleValues & GetNodeValues(NodeId nodeId) const
    {
      return particleValuesMap_.at(nodeId);
    }

    void
    Accumulate(NodeId nodeId, Accumulator & featuresAcc, Size n = 0) const;
    void
    Accumulate(NodeId nodeId, DensityAccumulator & densAcc, Size n = 0) const;
    void
        Accumulate(NodeId nodeId, QuantileAccumulator & quantAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, DiscreteAccumulator & featuresAcc, Size n =
        0) const;
    void Accumulate(NodeId nodeId,
                    ArrayAccumulator & featuresAcc,
                    const DimArray::Ptr & pDim) const;

    void ClearNode(NodeId nodeId)
    {
      particleValuesMap_.erase(nodeId);
    }
  };

  class FilterMonitor: public Monitor
  {
  public:
    typedef FilterMonitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Monitor BaseType;

  protected:
    Bool resampled_;
    Scalar logNormConst_;

  public:
    FilterMonitor(Size iter, const Types<NodeId>::Array & sampledNodes) :
      BaseType(iter, sampledNodes)
    {
    }
    virtual ~FilterMonitor()
    {
    }

    void Init(const Types<Particle>::Array & particles,
              Scalar ess,
              Scalar sumOfWeights,
              Bool resampled,
              Scalar logNormConst);
    void AddNode(NodeId nodeId,
                 const Types<Particle>::Array & particles,
                 Size iter,
                 Bool discrete);

    Bool GetResampled() const
    {
      checkWeightsSet();
      return resampled_;
    }
    Scalar GetLogNormConst() const
    {
      checkWeightsSet();
      return logNormConst_;
    }
  };

  class SmoothMonitor: public Monitor
  {
  public:
    typedef SmoothMonitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Monitor BaseType;

    SmoothMonitor(Size iter, const Types<NodeId>::Array & sampledNodes) :
      BaseType(iter, sampledNodes)
    {
    }
    virtual ~SmoothMonitor()
    {
    }

    void Init(const ValArray & weights, Scalar ess, Scalar sumOfWeights);
    void AddNode(NodeId nodeId, const Monitor & filterMonitor);
  };

}

#endif /* BIIPS_MONITOR_HPP_ */
