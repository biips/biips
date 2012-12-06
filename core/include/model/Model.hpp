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

/*! \file Model.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MODEL_HPP_
#define BIIPS_MODEL_HPP_

#include "graph/Graph.hpp"
#include "sampler/ForwardSampler.hpp"
#include "sampler/BackwardSmoother.hpp"
#include "model/Monitor.hpp"
#include "common/Accumulator.hpp"

namespace Biips
{

  class Model
  {
  protected:
    boost::scoped_ptr<Graph> pGraph_;
    boost::scoped_ptr<ForwardSampler> pSampler_;
    boost::scoped_ptr<BackwardSmoother> pSmoother_;
    Types<boost::shared_ptr<Monitor> >::Array filterMonitors_;
    std::map<NodeId, Monitor *> filterMonitorsMap_;
    Types<boost::shared_ptr<Monitor> >::Array backwardSmoothMonitors_;
    std::map<NodeId, Monitor *> backwardSmoothMonitorsMap_;
    boost::scoped_ptr<Monitor> pGenTreeSmoothMonitor_;
    std::set<NodeId> genTreeSmoothMonitoredNodeIds_;
    Bool defaultMonitorsSet_;

    MultiArray extractMonitorStat(
        NodeId nodeId, StatTag statFeature,
        const std::map<NodeId, Monitor *> & monitorsMap) const;
    Histogram extractMonitorPdf(
        NodeId nodeId, Size numBins, Scalar cacheFraction,
        const std::map<NodeId, Monitor *> & monitorsMap) const;

  public:

    Model(Bool dataModel = false)
        : pGraph_(new Graph(dataModel)), defaultMonitorsSet_(false)
    {
    }
    virtual ~Model()
    {
    }

    Graph & graph()
    {
      return *pGraph_;
    }

    void SetDefaultFilterMonitors();

    Bool SetFilterMonitor(NodeId nodeId);
    Bool SetGenTreeSmoothMonitor(NodeId nodeId);
    Bool SetBackwardSmoothMonitor(NodeId nodeId);

    Bool SamplerBuilt() const
    {
      return pSampler_ && pSampler_->Built();
    }
    const ForwardSampler & Sampler() const;

    void ClearSampler()
    {
      pSampler_.reset();
      pSmoother_.reset();
    }

    void BuildSampler();

    void InitSampler(Size nParticles, Rng * pRng,
                     const String & rsType, Scalar threshold);
    void IterateSampler();

    Bool SmootherInitialized() const
    {
      return pSmoother_ && pSmoother_->Initialized();
    }
    const BackwardSmoother & Smoother() const;

    void InitBackwardSmoother();

    void IterateBackwardSmoother();

    // TODO manage multi statFeature
    MultiArray ExtractFilterStat(NodeId nodeId, StatTag statFeature) const;
    MultiArray ExtractGenTreeSmoothStat(NodeId nodeId, StatTag statFeature) const;
    MultiArray ExtractBackwardSmoothStat(NodeId nodeId, StatTag statFeature) const;

    Histogram ExtractFilterPdf(NodeId nodeId, Size numBins = 40,
                               Scalar cacheFraction = 0.25) const;
    Histogram ExtractGenTreeSmoothPdf(NodeId nodeId, Size numBins = 40,
                                   Scalar cacheFraction = 0.25) const;
    Histogram ExtractBackwardSmoothPdf(NodeId nodeId, Size numBins = 40,
                               Scalar cacheFraction = 0.25) const;

    // release_only flag: only release monitor objects but keep nodeIds
    void virtual ClearFilterMonitors(Bool release_only = false);
    void virtual ClearGenTreeSmoothMonitors(Bool release_only = false);
    void virtual ClearBackwardSmoothMonitors(Bool release_only = false);

    Scalar GetLogPriorDensity(NodeId nodeId) const;
  };

}

#endif /* BIIPS_MODEL_HPP_ */
